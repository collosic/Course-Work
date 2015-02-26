package types;

import java.util.HashMap;
import java.util.List;

import crux.Symbol;
import ast.*;
import ast.Comparison.Operation;

public class TypeChecker implements CommandVisitor {
    
    private HashMap<Command, Type> typeMap;
    private StringBuffer errorBuffer;
    private FunctionDefinition functionNode;
    

    /* Useful error strings:
     *
     * "Function " + func.name() + " has a void argument in position " + pos + "."
     * "Function " + func.name() + " has an error in argument in position " + pos + ": " + error.getMessage()
     *
     * "Function main has invalid signature."
     *
     * "Not all paths in function " + currentFunctionName + " have a return."
     *
     * "IfElseBranch requires bool condition not " + condType + "."
     * "WhileLoop requires bool condition not " + condType + "."
     *
     * "Function " + currentFunctionName + " returns " + currentReturnType + " not " + retType + "."
     *
     * "Variable " + varName + " has invalid type " + varType + "."
     * "Array " + arrayName + " has invalid base type " + baseType + "."
     */

    public TypeChecker()
    {
        typeMap = new HashMap<Command, Type>();
        errorBuffer = new StringBuffer();
    }

    private void reportError(int lineNum, int charPos, String message)
    {
        errorBuffer.append("TypeError(" + lineNum + "," + charPos + ")");
        errorBuffer.append("[" + message + "]" + "\n");
    }

    private void put(Command node, Type type)
    {
        if (type instanceof ErrorType) {
            reportError(node.lineNumber(), node.charPosition(), ((ErrorType)type).getMessage());
        }
        typeMap.put(node, type);
    }
    
    public Type getType(Command node)
    {
        return typeMap.get(node);
    }
    
    public boolean check(Command ast)
    {
        ast.accept(this);
        return !hasError();
    }
    
    public boolean hasError()
    {
        return errorBuffer.length() != 0;
    }
    
    public String errorReport()
    {
        return errorBuffer.toString();
    }

    @Override
    public void visit(ExpressionList node) {  	
		for (Expression e : node) {
			Command c = (Command) e;
			check(c);
			Type t = getType(c);
			if (!(t instanceof ErrorType)) {
				put(node, getType(c));
			} 
			
		}
    }

    @Override
    public void visit(DeclarationList node) {
    	for (Declaration d : node) {
    		check((Command) d);
    			
    	}
    }

    @Override
    public void visit(StatementList node) {
    	TypeList ifelse_list = new TypeList();
    	for (Statement s : node) {
			check((Command) s);
			
			if (s instanceof ast.IfElseBranch) {
				ifelse_list.append(getType((Command) s));
			}
			
			if (s instanceof ast.Return && !(getType((Command) s) instanceof ErrorType))
				put(node, getType((Command) s));
    	}
    	// at this point we need to check to see if we have any VoidType returns in out 
    	// ifelse typelist, IF our last statement did not return a type
    	Type has_return = getReturnType(node);
    	if (has_return == null) has_return = new VoidType();
    	
    	// if has_return has a VoidType then we did get a Return node and we need to 
    	// check all paths for return types
    	if (has_return instanceof VoidType) {
        	for (Type t : ifelse_list) {
        		// if we enter the following if statement it means we did not have a 
        		// return node and there was a VoidType in one of the paths
        		if (t instanceof VoidType) {
        			put(node, t);
        			break;
        		}
        	}
        	if (getType(node) == null)
        		put(node, new VoidType());
    	}

    }

    @Override
    public void visit(AddressOf node) {
        // This is another leaf Node
    	Type t = node.symbol().type();
    	put(node, new AddressType(t));
    }

    @Override
    public void visit(LiteralBool node) {
        put(node, Type.getBaseType("bool"));
    }

    @Override
    public void visit(LiteralFloat node) {
    	put(node, Type.getBaseType("float"));
    }

    @Override
    public void visit(LiteralInt node) {
        // This is a leaf Node
    	put(node, Type.getBaseType("int"));
    }

    @Override
    public void visit(VariableDeclaration node) {
    	put(node, node.symbol().type());	
    }

    @Override
    public void visit(ArrayDeclaration node) {
    	put(node, node.symbol().type());  
    }

    @Override
    public void visit(FunctionDefinition node) { 
    	// check and see if the main function has the correct signature of return type void
    	String currentFunctionName = node.function().name();
    	Type currentReturnType = node.function().type();
    	FuncType func_t = createFuncType(node, currentReturnType);
    	functionNode = node;
    	
    	if (currentFunctionName.equals("main") && !currentReturnType.equivalent(Type.getBaseType("void"))) {
    		put(node, new ErrorType("Function main has invalid signature."));
    	}
    	
		checkArguments(func_t, node);
		
		check(node.body());
		Type retType = getReturnType(node.body());
		if (retType == null) {
			retType = Type.getBaseType("void");
			put(node, currentReturnType);
		} else if (retType instanceof ErrorType && currentFunctionName.equals("main")) {
	    	functionReturnError(node);
		} else if (retType instanceof VoidType && !currentFunctionName.equals("main") && 
							hasOtherPaths(node.body())){
			put(node, new ErrorType("Not all paths in function " + currentFunctionName + 
									" have a return."));
		} else if (!currentReturnType.equivalent(retType) && !(retType instanceof ErrorType)) {
			functionReturnError(node);
			
		} else {
			put(node, currentReturnType);
		}

    }

    @Override
    public void visit(Comparison node) {
    	Command leftside = (Command) node.leftSide();
    	Operation operation = node.operation();
    	Command rightside = (Command) node.rightSide();
    	
        check(leftside);
        check(rightside);
        
        Type left_t = getType(leftside);
        Type right_t = getType(rightside);
        
        
        // this bool types are used for testing only
        BoolType bt = new BoolType();
        
        if (!left_t.equivalent(right_t)) {
        	put(node, left_t.compare(right_t));
        } else if (bt.equivalent(left_t) || bt.equivalent(right_t)) {
        	put(node, left_t.compare(right_t));
        } else {
        	put(node, bt);
        }
        
        
    	
    }
    
    @Override
    public void visit(Addition node) {
    	Command leftside = (Command) node.leftSide();
    	Command rightside = (Command) node.rightSide();
    	
        check(leftside);
        check(rightside);
        
        Type left_t = getType(leftside);
        Type right_t = getType(rightside);
        
        if (!left_t.equivalent(right_t)) {
        	put(node, left_t.add(right_t));
        } else if (checkReturnTypes(left_t, right_t)){
        	// if they are equivalent then we can use either one in the hash map
        	put(node, left_t.add(right_t));	
        } else {
        	put(node, left_t);
        }
    }
    
    @Override
    public void visit(Subtraction node) {
        throw new RuntimeException("Implement this");
    }
    
    @Override
    public void visit(Multiplication node) {
        throw new RuntimeException("Implement this");
    }
    
    @Override
    public void visit(Division node) {
        throw new RuntimeException("Implement this");
    }
    
    @Override
    public void visit(LogicalAnd node) {
    	Command leftside = (Command) node.leftSide();
    	Command rightside = (Command) node.rightSide();
    	
        check(leftside);
        put(leftside, getType(leftside));
        check(rightside);
        put(rightside, getType(rightside));
        
        Type left_t = getType(leftside);
        Type right_t = getType(rightside);
              
        // this bool types are used for testing only
        BoolType bt = new BoolType();
        
        if (!left_t.equivalent(right_t)) {
        	put(node, left_t.and(right_t));
        } else if (!bt.equivalent(left_t) && !bt.equivalent(right_t)) {
        	put(node, left_t.and(right_t));
        } else {
        	put(node, bt);
        }

    }

    @Override
    public void visit(LogicalOr node) {
        throw new RuntimeException("Implement this");
    }

    @Override
    public void visit(LogicalNot node) {
    	Command c = (Command) node.expression();
    	
    	check(c);
    	Type t = getType(c);
    	
    	if (t instanceof AddressType)
    		t = ((AddressType) t).base();
    	
    	// this bool types are used for testing only
        BoolType bt = new BoolType();
        
    	if (bt.equivalent(t)) {
    		put(node, t);
    	} else {
    		put(node, t.not());
    	}
    	
    }
    
    @Override
    public void visit(Dereference node) {
    	Command c = (Command) node.expression();
    	check(c);
    	
    	// let's deference the variable
    	Type t = getType(c);
    	
    	if (t instanceof ErrorType) {
    		put(node, t.deref());
    	} else {
    		if (t instanceof AddressType) {
    			put(node, ((AddressType) t).base());
    		} else {
    			put(node, t);
    		}
    	}
    }

    @Override
    public void visit(Index node) {
    	Command c = (Command) node.base();
    	Command amount = (Command) node.amount();
    	check(c);
    	
    	Type t = getType(c);
    	Type base = t;
    	
    	if (t instanceof AddressType) {
    		base = ((AddressType) t).base();
    		int extent = ((ArrayType) base).extent();
    		Type new_base = ((ArrayType) base).base();
    		put(node, new_base);
    		
    	}
    	if (t instanceof ArrayType) {
    		put(node, ((ArrayType) t).base());
    	}
    	
    	if (!(t instanceof ArrayType) && !(t instanceof AddressType)) {
    		AddressType at = new AddressType(t);
    		put(node, at.index(t));
    	}
    	
    	
    	
   	
    }

    @Override
    public void visit(Assignment node) {
    	Command dest = (Command) node.destination();
    	Command source = (Command) node.source();
    	
    	check(dest);
    	check(source);
    	Type d = getType(dest);
    	Type s = getType(source);
    	Type d_type = d;
    	Type s_type = s;
    	
       	if (d instanceof AddressType)
    		d_type = ((AddressType) d).base();
       	
       	if (d instanceof ArrayType)
    		d_type = ((ArrayType) d).base();
       	
       	if (s instanceof AddressType)
    		s_type = ((AddressType) s).base();
       	
    	if (s instanceof AddressType)
    		s_type = ((AddressType) s).base();
    	
    	
    	if (s_type.equivalent(d_type)) {
    		put(node, d);
    	} else {
    		put(node, d.assign(s));
    	}
    	
    }

    @Override
    public void visit(Call node) {	
    	check(node.arguments());
    	TypeList func_args = new TypeList();
    	TypeList func_call_args = new TypeList();
    	
    	for (Expression e : node.arguments()) {
    		Type t = getType((Command) e);
    		func_call_args.append(t);
    	}
    	
      	Type funcReturnType = node.function().type();
    	String name = node.function().name();
    	Type t = null;
    	
    	if (name.equals("printBool")) {
    		t = new BoolType();
    	} else if (name.equals("printInt")) {
    		t = new IntType();
    	} else if (name.equals("printFloat")) {
    		t = new FloatType();
    	} else {
    		t = getType(node.arguments());
    	}
    	
    	func_args.append(t);
    	for(Type e : func_call_args) {
    		if (!t.equivalent(e)) {
    			if (e instanceof ArrayType)
    				e = ((ArrayType) e).base();
    			FuncType ft = new FuncType(func_args, funcReturnType, name);
        		put(node, ft.call(func_call_args));
    		}
    	}
    	   	  	
    	put(node, funcReturnType);
    }

    @Override
    public void visit(IfElseBranch node) {
    	Type retType = null;
    	Command condition = (Command) node.condition();
    	
    	check(condition);
    	Type t = getType(condition);
    	if (t instanceof AddressType)
    		t = ((AddressType) t).base();
    	
    	// test if the condition is a boolType
    	BoolType bt = new BoolType();
    	
    	if (!bt.equivalent(t)) {
    		put(node, new ErrorType("IfElseBranch requires bool condition not " + t + "."));
    	} 
    	
    	StatementList then_block =  node.thenBlock();
    	StatementList else_block = node.elseBlock();
    	
    	check(then_block);
   	
    	if (else_block.size() > 0) {
	    	check(else_block);
	    	// we can check to see that both return types are the same

	    	
	    	// here we can check for both statement blocks
	    	if (getType(then_block) instanceof VoidType) {
	    		retType = getType(then_block);
	    	} else if (getType(else_block) instanceof VoidType) {
	    		retType = getType(else_block);
	    	} else {
	    		// we can use either return since at this point they should both return the same 
	    		// type
	    		retType = getType(then_block);
	    	}
    	} else {
    		// we don't have an else block and we only need to check the if_block
    		retType = getType(then_block) instanceof VoidType ? getType(then_block) : new VoidType();
    	}
    	
    	// now check to see if we have returns in the paths, if they exist
    	put(node, retType);

    }

    @Override
    public void visit(WhileLoop node) {
    	Command condition = (Command) node.condition();
    	check(condition);
    	Type t = getType(condition);
    	if (t instanceof AddressType)
    		t = ((AddressType) t).base();
    	// test if the condition is a boolType
    	BoolType bt = new BoolType();
    	
    	if (!bt.equivalent(t)) {
    		put(node, new ErrorType("WhileLoop requires bool condition not " + t + "."));
    	} else {
    		put(node, t);
    	}
    }

    @Override
    public void visit(Return node) {
    	Command c = (Command) node.argument();
    	
    	check(c);
	    Type t = getType(c);
    	if (t instanceof AddressType)
    		t = ((AddressType) t).base();
    	
	    if (t instanceof ErrorType) {
	    	functionReturnError(functionNode, c, node);
	    } else {
	    	if (t instanceof AddressType) {
	    		t = ((AddressType) t).base();
	    	}
	    	if (t instanceof ArrayType) {
	    		t = ((ArrayType) t).base();
	    	}
	    	if (t instanceof FuncType) {
	    		t = ((FuncType) t).returnType();
	    	}
	    	put(node, t);
	    } 
    }

    @Override
    public void visit(ast.Error node) {
        put(node, new ErrorType(node.message()));
    }
    
    private FuncType createFuncType(FunctionDefinition node, Type r) {
    	TypeList types = new TypeList();
    	for(Symbol s : node.arguments()) {
    		types.append(s.type());
    	}   	
    	return new FuncType(types, r, node.function().name());
    }
    
    private void functionReturnError(FunctionDefinition node) {
    	String currentFunctionName = node.function().name();
    	Type currentReturnType = node.function().type();
    	Type retType = getType(node.body());
    	String func = "Function " + currentFunctionName + " returns " + 
				currentReturnType + " not " + retType + ".";
    	  	
    	put(node.body(), new ErrorType(func));
    	
    }
    
    // These are all helper functions for errors the have occurred
    private void functionReturnError(FunctionDefinition node, Command ret, Command retn) {
    	String currentFunctionName = node.function().name();
    	Type currentReturnType = node.function().type();
    	Type retType = getType(ret);
    	String func = "Function " + currentFunctionName + " returns " + 
				currentReturnType + " not " + retType + ".";
    	  	
    	put(retn, new ErrorType(func));
    	
    }
    
    private void checkArguments(FuncType func, FunctionDefinition node) {
    	VoidType vt = new VoidType();
    	ErrorType et = null;
    	TypeList types = func.arguments();
    	int pos = 0;
    	
    	for(Type t : types) {   		    		
    		if (vt.equivalent(t)) {
    			put(node, new ErrorType("Function " + func.name() + 
    									" has a void argument in position " + pos + "."));
    		}
    		
    		if (t instanceof ErrorType) {
    			et = (ErrorType) t;
    			put(node, new ErrorType("Function " + func.name() + " has an error in argument in position " + 
    					pos + ": " + et.getMessage()));
    		}
    		pos++;
    	}
    }
    
   
    private boolean checkReturnTypes(Type l, Type r) {
    	// let's check the left and right sides and see if they can
    	IntType it = new IntType();
    	FloatType ft = new FloatType();
    	
    	if ((it.equivalent(l) && it.equivalent(r)) || (ft.equivalent(l) && ft.equivalent(r))) {
    		return false;
    	} else {
    		return true;
    	}
    }
    
    private boolean hasReturns(StatementList node) {
    	for (Statement s : node) {
    		if (s instanceof ast.Return)
    			return true;    		
    	}
    	return false;
    }
    
    private Type getReturnType(StatementList node) {
    	// we need to check for the last element in the statement list
    	if (hasReturns(node)) {
    		Type t = null;
        	for (Statement s : node) {
        		if (s instanceof ast.Return) {
        			t = getType((Command) s);
        			break;
        		}
        	}
        	return t;
    	}
    	return getType((Command) node);
    }
    
    private boolean hasOtherPaths(StatementList node) {
    	// check and see if other retun paths exist
    	for (Statement s : node) {
			if (s instanceof ast.IfElseBranch || s instanceof ast.WhileLoop) {
				return true;
			}
    	}
    	return false;
    }
    
    private Type getArrayType(Type t) {
		Type base = ((AddressType) t).base();
   		if (base instanceof ArrayType) {
   			base = ((ArrayType) base).base();
   			if(base instanceof ArrayType)
   				base = ((ArrayType) base).base();
   		} 
   		return base;
    }
}
