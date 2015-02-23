package crux;

import java.util.ArrayList;
import java.util.List;
import java.util.Stack;

import types.Type;
import types.TypeList;
import ast.Command;
import ast.Expression;

public class Parser {
    public static String studentName = "Christian Collosi";
    public static String studentID = "11233529";
    public static String uciNetID = "ccollosi@uci.edu";
    
    
// Typing System ===================================
    
    private Type tryResolveType(String typeStr)
    {
        return Type.getBaseType(typeStr);
    }
        
    private int expectInteger() {

    	return 0;
    }

    
// SymbolTable Management ==========================
    private SymbolTable symbolTable;
    
    private void initSymbolTable()
    {
    	symbolTable = new SymbolTable();
    	Symbol sym = symbolTable.insert("readInt");
    	sym.setType(Type.getBaseType("int"));
    	sym = symbolTable.insert("readFloat");
    	sym.setType(Type.getBaseType("float"));
    	sym =symbolTable.insert("printBool");
    	sym.setType(Type.getBaseType("void"));
    	sym = symbolTable.insert("printInt");
    	sym.setType(Type.getBaseType("void"));
    	sym = symbolTable.insert("printFloat");
    	sym.setType(Type.getBaseType("void"));
    	sym = symbolTable.insert("println");
    	sym.setType(Type.getBaseType("void"));
    }
    
    private void enterScope()
    {
    	// we need to link the parent symbol table with this new symbol table 
    	// if a parent table exits
    	int prevDepth;
   		SymbolTable prevSymbolTable = symbolTable;
   		prevDepth = prevSymbolTable.getDepth();
   		symbolTable = new SymbolTable(prevDepth + 1);
   		symbolTable.setParent(prevSymbolTable);
    }
    
    private void exitScope()
    {
    	if (symbolTable.parentExists()) {
    		symbolTable = symbolTable.getParent();
    	}
    }

    private Symbol tryResolveSymbol(Token ident)
    {
        assert(ident.is(Token.Kind.IDENTIFIER));
        String name = ident.lexeme();
        try {
            return symbolTable.lookup(name);
        } catch (SymbolNotFoundError e) {
            String message = reportResolveSymbolError(name, ident.lineNumber(), ident.charPosition());
            return new ErrorSymbol(message);
        }
    }

    private String reportResolveSymbolError(String name, int lineNum, int charPos)
    {
        String message = "ResolveSymbolError(" + lineNum + "," + charPos + ")[Could not find " + name + ".]";
        errorBuffer.append(message + "\n");
        errorBuffer.append(symbolTable.toString() + "\n");
        return message;
    }

    private Symbol tryDeclareSymbol(Token ident)
    {
        assert(ident.is(Token.Kind.IDENTIFIER));
        String name = ident.lexeme();
        try {
            return symbolTable.insert(name);
        } catch (RedeclarationError re) {
            String message = reportDeclareSymbolError(name, ident.lineNumber(), ident.charPosition());
            return new ErrorSymbol(message);
        }
    }

    private String reportDeclareSymbolError(String name, int lineNum, int charPos)
    {
        String message = "DeclareSymbolError(" + lineNum + "," + charPos + ")[" + name + " already exists.]";
        errorBuffer.append(message + "\n");
        errorBuffer.append(symbolTable.toString() + "\n");
        return message;
    }
    
// Error Reporting ==========================================
    private StringBuffer errorBuffer = new StringBuffer();
    
    private String reportSyntaxError(NonTerminal nt)
    {
        String message = "SyntaxError(" + lineNumber() + "," + charPosition() + ")[Expected a token from " + nt.name() + " but got " + currentToken.kind() + ".]";
        errorBuffer.append(message + "\n");
        return message;
    }
     
    private String reportSyntaxError(Token.Kind kind)
    {
        String message = "SyntaxError(" + lineNumber() + "," + charPosition() + ")[Expected " + kind + " but got " + currentToken.kind() + ".]";
        errorBuffer.append(message + "\n");
        return message;
    }
    
    public String errorReport()
    {
        return errorBuffer.toString();
    }
    
    public boolean hasError()
    {
        return errorBuffer.length() != 0;
    }
    
    private class QuitParseException extends RuntimeException
    {
        private static final long serialVersionUID = 1L;
        public QuitParseException(String errorMessage) {
            super(errorMessage);
        }
    }
    
    private int lineNumber()
    {
        return currentToken.lineNumber();
    }
    
    private int charPosition()
    {
        return currentToken.charPosition();
    }
    
    
    
// Parser ==========================================
    private Scanner scanner;
    private Token currentToken; 
    
    public Parser(Scanner scanner)
    {
        this.scanner = scanner;
        currentToken = scanner.next();
    }
   
    public ast.Command parse()
    {
        initSymbolTable();
        try {
            return program();
        } catch (QuitParseException q) {
            return new ast.Error(lineNumber(), charPosition(), "Could not complete parsing.");
        }
    }
    
// Helper Methods ==========================================
    private boolean have(Token.Kind kind)
    {
        return currentToken.is(kind);
    }
    
    private boolean have(NonTerminal nt)
    {
        return nt.firstSet().contains(currentToken.kind());
    }
    
    private boolean accept(Token.Kind kind)
    {
        if (have(kind)) {
            currentToken = scanner.next();
            return true;
        }
        return false;
    }    
    
    private boolean accept(NonTerminal nt)
    {
        if (have(nt)) {
            currentToken = scanner.next();
            return true;
        }
        return false;
    }

    private boolean expect(Token.Kind kind)
    {
        if (accept(kind))
            return true;
        String errormessage = reportSyntaxError(kind);
        throw new QuitParseException(errormessage);
        //return false;
    }
        
    private boolean expect(NonTerminal nt)
    {
        if (accept(nt))
            return true;
        String errormessage = reportSyntaxError(nt);
        throw new QuitParseException(errormessage);
        //return false;
    }
     
    private Token expectRetrieve(Token.Kind kind)
    {
        Token tok = currentToken;
        if (accept(kind))
            return tok;
        String errorMessage = reportSyntaxError(kind);
        throw new QuitParseException(errorMessage);
        //return ErrorToken(errorMessage);
    }
        
    private Token expectRetrieve(NonTerminal nt)
    {
        Token tok = currentToken;
        if (accept(nt))
            return tok;
        String errorMessage = reportSyntaxError(nt);
        throw new QuitParseException(errorMessage);
        //return ErrorToken(errorMessage);
    }

// Grammar Rules =====================================================
    // Used to store the parameters of a function 
    ArrayList<Symbol> params = new ArrayList<Symbol>();
    
    // literal := INTEGER | FLOAT | TRUE | FALSE .
    public ast.Expression literal()
    {
        ast.Expression expr;
        if (have(Token.Kind.INTEGER) || have(Token.Kind.FLOAT) ||
                have(Token.Kind.TRUE) || have(Token.Kind.FALSE)) {
                          
                Token tok = expectRetrieve(NonTerminal.LITERAL);
                expr = Command.newLiteral(tok);
            } else {
                throw new QuitParseException(reportSyntaxError(NonTerminal.LITERAL));
            }

        return expr;
    }
    
    // designator := IDENTIFIER { "[" expression0 "]" } .
    public ast.Expression designator()
    {
        
        Token tok =	expectRetrieve(Token.Kind.IDENTIFIER);
        Symbol sym = tryResolveSymbol(tok);
        
        ast.Index index = null;
        ast.AddressOf addy;
        addy = new ast.AddressOf(tok.lineNumber(), tok.charPosition(), sym);
        
        while (accept(Token.Kind.OPEN_BRACKET)) {
        	tok = currentToken;
            ast.Expression amount = expression0();
            if (index == null) {
            	index = new ast.Index(tok.lineNumber(), tok.charPosition(), addy, amount);
            } else {
            	index = new ast.Index(tok.lineNumber(), tok.charPosition(), index, amount);
            }
            
            expect(Token.Kind.CLOSE_BRACKET);
        }
        if (index == null) {
        	return addy;
        }
        return index;
    }
    
    // type := IDENTIFIER .
    public Type type()
    { 	
        Token tok = expectRetrieve(Token.Kind.IDENTIFIER);
        return tryResolveType(tok.lexeme());
    }
    
    // op0 := ">=" | "<=" | "!=" | "==" | ">" | "<" .
    public Token op0()
    {
    	Token tok;
        if (have(Token.Kind.GREATER_EQUAL)) {
        	tok = expectRetrieve(Token.Kind.GREATER_EQUAL);
        } else if (have(Token.Kind.LESSER_EQUAL)) {
        	tok = expectRetrieve(Token.Kind.LESSER_EQUAL);
        } else if (have(Token.Kind.NOT_EQUAL)) {
        	tok = expectRetrieve(Token.Kind.NOT_EQUAL);
        } else if (have(Token.Kind.EQUAL)) {
        	tok = expectRetrieve(Token.Kind.EQUAL); 
        } else if (have(Token.Kind.GREATER_THAN)) {
        	tok = expectRetrieve(Token.Kind.GREATER_THAN);
        } else if (have(Token.Kind.LESS_THAN)) {
        	tok = expectRetrieve(Token.Kind.LESS_THAN);
        } else {
        	throw new QuitParseException(reportSyntaxError(NonTerminal.OP0));
        }
        return tok;
    }
    
    // op1 := "+" | "-" | "or" .
    public Token op1()
    {
    	Token tok;
        if (have(Token.Kind.ADD)) {
            tok = expectRetrieve(Token.Kind.ADD);
        } else if (have(Token.Kind.SUB)) {
        	tok = expectRetrieve(Token.Kind.SUB);
        } else if (have(Token.Kind.OR)) {
        	tok = expectRetrieve(Token.Kind.OR);
        } else {
        	throw new QuitParseException(reportSyntaxError(NonTerminal.OP1));
        }
        return tok;
    }
    
    // op2 := "*" | "/" | "and" .
    public Token op2()
    {
    	Token tok;
        if (have(Token.Kind.MUL)) {
        	tok = expectRetrieve(Token.Kind.MUL);
        } else if (have(Token.Kind.DIV)) {
        	tok = expectRetrieve(Token.Kind.DIV);
        } else if (have(Token.Kind.AND)) {
        	tok = expectRetrieve(Token.Kind.AND);
        } else {
        	throw new QuitParseException(reportSyntaxError(NonTerminal.OP2));
        }
        return tok;
    }
    
    // expression0 := expression1 [ op0 expression1 ] .
    public ast.Expression expression0()
    {
    	ast.Expression exp, left_exp;
        // this call is a must
    	exp = left_exp = expression1();
        
        // we now have a option to continue calling once more to op0 and expression1
        if (have(NonTerminal.OP0)) {
            Token tok = op0();
            ast.Expression right_exp = expression1();
            exp = left_exp =  Command.newExpression(left_exp, tok, right_exp);

        }
        return exp;
    }
    
    // expression1 := expression2 { op1  expression2 } .
    public ast.Expression expression1()
    {
    	ast.Expression exp, left_exp;
		exp = left_exp = expression2();
        
        while (have(NonTerminal.OP1)) {
            Token tok = op1();
            ast.Expression right_exp = expression2();
            exp = left_exp =  Command.newExpression(left_exp, tok, right_exp);
        }
        return exp;
    }

    // expression2 := expression3 { op2 expression3 } .
    public ast.Expression expression2()
    {
    	ast.Expression exp, left_exp;
    	exp = left_exp = expression3();
        while (have(NonTerminal.OP2)) {
            Token tok = op2();
            ast.Expression right_exp = expression3();
            exp = left_exp =  Command.newExpression(left_exp, tok, right_exp);    
        } 
        return exp;
    }
    
    // expression3 := "not" expression3
    // | "(" expression0 ")"
    // | designator
    // | call-expression
    // | literal .
    public ast.Expression expression3()
    {
    	ast.Expression exp = null;
    	int lineNum = currentToken.lineNumber();
    	int charPos = currentToken.charPosition();
        if (have(Token.Kind.NOT)) {
        	Token tok = expectRetrieve(Token.Kind.NOT);
        	ast.Expression left_exp = expression3();
            exp = Command.newExpression(left_exp, tok, null);
        } else if (accept(Token.Kind.OPEN_PAREN)) {
            exp = expression0();
            expect(Token.Kind.CLOSE_PAREN); 
        } else if (have(Token.Kind.CALL)) {
            exp = call_expression();
        } else if (have(NonTerminal.DESIGNATOR)) {
        	exp = designator();
        	exp = new ast.Dereference(lineNum, charPos, exp);
        } else if (have(NonTerminal.LITERAL)) {
            exp = literal();
        } else {
        	throw new QuitParseException(reportSyntaxError(NonTerminal.EXPRESSION3));
        }
        return exp;
    }
    
    // call-expression := "::" IDENTIFIER "(" expression-list ")" .
    public ast.Call call_expression() {
        // need to check if this function was previously declared 
        Token tok = expectRetrieve(Token.Kind.CALL);
        Symbol sym = tryResolveSymbol(expectRetrieve(Token.Kind.IDENTIFIER));
 
        expect(Token.Kind.OPEN_PAREN);
        ast.ExpressionList args = expression_list();
        expect(Token.Kind.CLOSE_PAREN);
        return new ast.Call(tok.lineNumber(), tok.charPosition(), sym, args);
    }
    
    // expression-list := [ expression0 { "," expression0 } ] .
    public ast.ExpressionList expression_list() {
    	ast.ExpressionList expl = new ast.ExpressionList(lineNumber(), charPosition());
        if (have(NonTerminal.EXPRESSION0)) {
            expl.add(expression0());
            while (accept(Token.Kind.COMMA)) {
            	expl.add(expression0());
            }
        }
        return expl;
    }
    
    // parameter := IDENTIFIER ":" type .
    public Type parameter() {	
    	Symbol sym = tryDeclareSymbol(expectRetrieve(Token.Kind.IDENTIFIER));
    	//sym.setType(sym);
        expect(Token.Kind.COLON);
        Type t = type();
        sym.setType(t);
        params.add(sym);
        return t;
    }
    
    
    // parameter-list := [ parameter { "," parameter } ] .
    public TypeList parameter_list() {
    	TypeList tl = new TypeList();
        if (have(NonTerminal.PARAMETER)) {
            tl.append(parameter());
            while (accept(Token.Kind.COMMA)) {
            	tl.append(parameter());
            }
        }
        return tl;
    }
    
    // variable-declaration := "var" IDENTIFIER ":" type ";"
    public ast.VariableDeclaration variable_declaration() {
    	
        Token tok = expectRetrieve(Token.Kind.VAR);
        Symbol sym = tryDeclareSymbol(expectRetrieve(Token.Kind.IDENTIFIER));
        expect(Token.Kind.COLON);
        sym.setType(type());
        expect(Token.Kind.SEMICOLON);
        return new ast.VariableDeclaration(tok.lineNumber(), tok.charPosition(), sym);
    }
    
    // array-declaration := "array" IDENTIFIER ":" type "[" INTEGER "]" { "[" INTEGER "]" } ";"
    public ast.ArrayDeclaration array_declaration() {
    	Token tok = expectRetrieve(Token.Kind.ARRAY);
        Symbol sym = tryDeclareSymbol(expectRetrieve(Token.Kind.IDENTIFIER));

        expect(Token.Kind.COLON);
        type();
        expect(Token.Kind.OPEN_BRACKET);
        expect(Token.Kind.INTEGER);
        expect(Token.Kind.CLOSE_BRACKET);
        
        while (accept(Token.Kind.OPEN_BRACKET)) {
            expect(Token.Kind.INTEGER);
            expect(Token.Kind.CLOSE_BRACKET);
        }
        expect(Token.Kind.SEMICOLON);       
        return new ast.ArrayDeclaration(tok.lineNumber(), tok.charPosition(), sym);
    }
    
    // function-definition := "func" IDENTIFIER "(" parameter-list ")" ":" type statement-block .
    public ast.Declaration function_definition() {
    	Token tok = expectRetrieve(Token.Kind.FUNC);
        Symbol sym = tryDeclareSymbol(expectRetrieve(Token.Kind.IDENTIFIER));
        
        expect(Token.Kind.OPEN_PAREN);
    	enterScope();
    	
        // must determine a way to continue even if parameter-list is empty
        TypeList tl = parameter_list();
        ArrayList<Symbol> args = new ArrayList<Symbol>(params);
        params.clear();
        
        expect(Token.Kind.CLOSE_PAREN);
        expect(Token.Kind.COLON);
        // Next is 
        sym.setType(type());
        ast.StatementList body = statement_block();
        exitScope();       
        return new ast.FunctionDefinition(tok.lineNumber(), tok.charPosition(), sym, args, body);
    }
    
    // declaration := variable-declaration | array-declaration | function-definition .
    public ast.Declaration declaration() {
    	ast.Declaration dec = null;
        if (have(NonTerminal.VARIABLE_DECLARATION)) {
            dec = variable_declaration();
        } else if (have(NonTerminal.ARRAY_DECLARATION)) {
            return array_declaration();
        } else if (have(NonTerminal.FUNCTION_DEFINITION)) {
            dec = function_definition();
        } else {
        	throw new QuitParseException(reportSyntaxError(NonTerminal.DECLARATION));
        }
        return dec;
    }
    
    // declaration-list := { declaration } .
    public ast.DeclarationList declaration_list() {
    	ast.DeclarationList decl = new ast.DeclarationList(lineNumber(), charPosition());
        while (have(NonTerminal.DECLARATION)) {
            decl.add(declaration());
        }       
        return decl;
    }
    
    // assignment-statement := "let" designator "=" expression0 ";"
    public ast.Assignment assignment_statment() {

        Token tok = expectRetrieve(Token.Kind.LET);
    	tryResolveSymbol(currentToken);
        ast.Expression dest = designator();
        expect(Token.Kind.ASSIGN);
        ast.Expression source = expression0();
        expect(Token.Kind.SEMICOLON);
        return new ast.Assignment(tok.lineNumber(), tok.charPosition(), dest, source);
    }
    
    // call-statement := call-expression ";"
    public ast.Call call_statement() {

        ast.Call call = call_expression();
        expect(Token.Kind.SEMICOLON);     
        return call;

    }
    
    // if-statement := "if" expression0 statement-block [ "else" statement-block ] .
    public ast.IfElseBranch if_statement() {
    	Token tok = expectRetrieve(Token.Kind.IF);
    	ast.Expression cond = expression0();
    	
        // enter a new scope for the if statement
    	enterScope();        
    	ast.StatementList thenBlock = statement_block();
    	ast.StatementList elseBlock = new ast.StatementList(lineNumber(), charPosition());
        exitScope();
        if (accept(Token.Kind.ELSE)) {
        	enterScope();
        	elseBlock = statement_block();
            exitScope();
        }

        return new ast.IfElseBranch(tok.lineNumber(), tok.charPosition(), cond, thenBlock, elseBlock);
    }
    
    // while-statement := "while" expression0 statement-block .
    public ast.WhileLoop while_statement() {

    	Token tok = expectRetrieve(Token.Kind.WHILE);
        ast.Expression cond = expression0();
    	enterScope();
        ast.StatementList body = statement_block();   
        exitScope();
        return new ast.WhileLoop(tok.lineNumber(), tok.charPosition(), cond, body);
    }
    
    // return-statement := "return" expression0 ";" .
    public ast.Return return_statement() {

        Token tok = expectRetrieve(Token.Kind.RETURN);
        ast.Expression arg = expression0();
        expect(Token.Kind.SEMICOLON);       
        return new ast.Return(tok.lineNumber(), tok.charPosition(), arg);
    }
    
    
    // statement := variable-declaration 
    // | call-statement  
    // | assignment-statement 
    // | if-statement 
    // | while-statement 
    // | return-statement .
    public ast.Statement statement() {
    	ast.Statement state;
        if (have(Token.Kind.VAR)) {
            state = variable_declaration();
        } else if (have(Token.Kind.CALL)) {
            state = call_statement();
        } else if (have(Token.Kind.LET)) {
            state = assignment_statment();
        } else if (have(Token.Kind.IF)){
        	state = if_statement();
        } else if (have(Token.Kind.WHILE)) {
        	state = while_statement();
        } else if (have(Token.Kind.RETURN)) {
        	state = return_statement();
        } else {
        	throw new QuitParseException(reportSyntaxError(NonTerminal.STATEMENT));	
        }
        return state;
    }
    
    // { statement } .
    public ast.StatementList statement_list() {
    	ast.StatementList body = new ast.StatementList(lineNumber(), charPosition());
        while(have(NonTerminal.STATEMENT)) {
            body.add(statement());
        }
        return body;
    }
    
    // "{" statement-list "}" .
    public ast.StatementList statement_block() {
        expect(Token.Kind.OPEN_BRACE);
        ast.StatementList bod = statement_list();
        expect(Token.Kind.CLOSE_BRACE);
        return bod;
        
    }
    
    // program := declaration-list EOF .
    public ast.DeclarationList program()
    {
    	ast.DeclarationList decl;
        decl = declaration_list();
        expect(Token.Kind.EOF);
        return decl;
    }
    
}
