package types;

import java.util.HashMap;
import java.util.List;

import crux.Symbol;
import ast.*;

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
			check((Command) e);
			put(node, getType((Command) e));
		}
    }

    @Override
    public void visit(DeclarationList node) {
    	for (Declaration d : node) {
    		if (!check((Command)d)) {
    			
    		}

    	}
    }

    @Override
    public void visit(StatementList node) {
    	for (Statement s : node) {
			check((Command) s);
			put(node, getType((Command) s));
    	}
    }

    @Override
    public void visit(AddressOf node) {
        // This is another leaf Node
    	put(node, node.symbol().type());
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
        throw new RuntimeException("Implement this");
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
    	} else {
    		checkArguments(func_t, node);
    		
    		check(node.body());
    		Type retType = getType(node.body());
	    	if(!currentReturnType.equivalent(retType) && !currentFunctionName.equals("main")) {
	    		functionReturnError(node);
	    	} else {
	    		put(node, currentReturnType);
	    	}	    		
    	}
    }

    @Override
    public void visit(Comparison node) {
        throw new RuntimeException("Implement this");
    }
    
    @Override
    public void visit(Addition node) {
    	Command leftside = (Command) node.leftSide();
    	Command rightside = (Command) node.rightSide();
    	
        check(leftside);
        put(leftside, getType(leftside));
        check(rightside);
        put(rightside, getType(rightside));
        
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
        throw new RuntimeException("Implement this");
    }

    @Override
    public void visit(LogicalOr node) {
        throw new RuntimeException("Implement this");
    }

    @Override
    public void visit(LogicalNot node) {
        throw new RuntimeException("Implement this");
    }
    
    @Override
    public void visit(Dereference node) {
    	Command c = (Command) node.expression();
    	check(c);

        put(node, getType(c));
    }

    @Override
    public void visit(Index node) {
        throw new RuntimeException("Implement this");
    }

    @Override
    public void visit(Assignment node) {
    	Command dest = (Command) node.destination();
    	Command source = (Command) node.source();
    	
    	check(dest);
    	check(source);
    	

    }

    @Override
    public void visit(Call node) {
    	check(node.arguments());
    	Type t = node.function().type();
    	put(node, t);
    }

    @Override
    public void visit(IfElseBranch node) {
        throw new RuntimeException("Implement this");
    }

    @Override
    public void visit(WhileLoop node) {
        throw new RuntimeException("Implement this");
    }

    @Override
    public void visit(Return node) {
    	Command c = (Command) node.argument();
    	
    	check(c);
	    Type t = getType(c);
	    if (checkForTypeErrors(t)) {
	    	functionReturnError(functionNode, c, node);
	    	
	    } else {
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
    		if (checkForTypeErrors(t)) {
    			et = (ErrorType) t;
    		}
    		    		
    		if (vt.equivalent(t)) {
    			put(node, new ErrorType("Function " + func.name() + 
    					" has a void argument in position " + pos + "."));
    		}
    		
    		if (et != null && et.equivalent(t)) {
    			put(node, new ErrorType("Function " + func.name() + " has an error in argument in position " + 
    					pos + ": " + et.getMessage()));
    		}
    		pos++;
    	}
    	
    
    }
    
    private boolean checkForTypeErrors(Type t) {
    	IntType it = new IntType();
    	FloatType ft = new FloatType();
    	BoolType bt = new BoolType();
    	VoidType vt = new VoidType();
    	
    	if (it.equivalent(t) || ft.equivalent(t) ||
    			bt.equivalent(t) || vt.equivalent(t)) {
    		return false;
    	} else {
    		return true;
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
    
    private boolean checkReturnType(Type t) {
    	IntType it = new IntType();
    	FloatType ft = new FloatType();
    	
    	if (it.equivalent(t) || ft.equivalent(t)) {
    		return true;
    	}
    	return false;
    }
    
}
