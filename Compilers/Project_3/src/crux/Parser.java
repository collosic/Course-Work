package crux;

import java.util.ArrayList;
import java.util.List;
import java.util.Stack;

public class Parser {
    public static String studentName = "Christian Collosi";
    public static String studentID = "11233529";
    public static String uciNetID = "ccollosi@uci.edu";
    
// SymbolTable Management ==========================
    private SymbolTable symbolTable;
    
    private void initSymbolTable()
    {
    	symbolTable = new SymbolTable();
    	symbolTable.enterSymbol("readInt", "readInt");
    	symbolTable.enterSymbol("readFloat", "readFloat");
    	symbolTable.enterSymbol("printBool", "printBool");
    	symbolTable.enterSymbol("printInt", "printInt");
    	symbolTable.enterSymbol("printFloat", "printFloat");
    	symbolTable.enterSymbol("println", "println");
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
        //throw new RuntimeException("implement this");
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

// Helper Methods ==========================================

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
        String errorMessage = reportSyntaxError(kind);
        throw new QuitParseException(errorMessage);
        //return false;
    }
        
    private boolean expect(NonTerminal nt)
    {
        if (accept(nt))
            return true;
        String errorMessage = reportSyntaxError(nt);
        throw new QuitParseException(errorMessage);
        //return false;
    }
              
// Parser ==========================================
    private Scanner scanner;
    private Token currentToken; 
    
    public Parser(Scanner scanner)
    {
        this.scanner = scanner;
        currentToken = scanner.next();
    }
    
    public void parse()
    {
        initSymbolTable();
        try {
            program();
        } catch (QuitParseException q) {
            errorBuffer.append("SyntaxError(" + lineNumber() + "," + charPosition() + ")");
            errorBuffer.append("[Could not complete parsing.]");
        }
    }
    
// Grammar Rules =====================================================
    
    // literal := INTEGER | FLOAT | TRUE | FALSE .
    public void literal()
    {
    	
        if (accept(Token.Kind.INTEGER) || accept(Token.Kind.FLOAT) ||
            accept(Token.Kind.TRUE) || accept(Token.Kind.FALSE)) {
            // I guess this is good         
        } else {
        	throw new QuitParseException(reportSyntaxError(NonTerminal.LITERAL));
        }
        
    }
    
    // designator := IDENTIFIER { "[" expression0 "]" } .
    public void designator()
    {
        expect(Token.Kind.IDENTIFIER);
        while (accept(Token.Kind.OPEN_BRACKET)) {
            expression0();
            expect(Token.Kind.CLOSE_BRACKET);
        }
        
    }
    
    // type := IDENTIFIER .
    public void type()
    {

        expect(Token.Kind.IDENTIFIER);
    }
    
    // op0 := ">=" | "<=" | "!=" | "==" | ">" | "<" .
    public void op0()
    {

        if (have(Token.Kind.GREATER_EQUAL)) {
            accept(Token.Kind.GREATER_EQUAL);
        } else if (have(Token.Kind.LESSER_EQUAL)) {
            accept(Token.Kind.LESSER_EQUAL);
        } else if (have(Token.Kind.NOT_EQUAL)) {
            accept(Token.Kind.NOT_EQUAL);
        } else if (have(Token.Kind.EQUAL)) {
            accept(Token.Kind.EQUAL); 
        } else if (have(Token.Kind.GREATER_THAN)) {
            accept(Token.Kind.GREATER_THAN);
        } else if (have(Token.Kind.LESS_THAN)) {
            accept(Token.Kind.LESS_THAN);
        } else {
        	throw new QuitParseException(reportSyntaxError(NonTerminal.OP0));
        }

    }
    
    // op1 := "+" | "-" | "or" .
    public void op1()
    {

        if (have(Token.Kind.ADD)) {
            accept(Token.Kind.ADD);
        } else if (have(Token.Kind.SUB)) {
            accept(Token.Kind.SUB);
        } else if (have(Token.Kind.OR)) {
            accept(Token.Kind.OR);
        } else {
        	throw new QuitParseException(reportSyntaxError(NonTerminal.OP1));
        }

    }
    
    // op2 := "*" | "/" | "and" .
    public void op2()
    {

        if (have(Token.Kind.MUL)) {
            accept(Token.Kind.MUL);
        } else if (have(Token.Kind.DIV)) {
            accept(Token.Kind.DIV);
        } else if (have(Token.Kind.AND)) {
            accept(Token.Kind.AND);
        } else {
        	throw new QuitParseException(reportSyntaxError(NonTerminal.OP2));
        }
        
    }
    
    // expression0 := expression1 [ op0 expression1 ] .
    public void expression0()
    {

        // this call is a must
        expression1();
        
        // we now have a option to continue calling once more to op0 and expression1
        if (have(NonTerminal.OP0)) {
            op0();
            expression1();
        }
        
    }
    
    // expression1 := expression2 { op1  expression2 } .
    public void expression1()
    {

        expression2();
        
        while (have(NonTerminal.OP1)) {
            op1();
            expression2();
        }

    }

    // expression2 := expression3 { op2 expression3 } .
    public void expression2()
    {
        
        expression3();
        while (have(NonTerminal.OP2)) {
            op2();
            expression3();
        }
        

    }
    
    // expression3 := "not" expression3
    // | "(" expression0 ")"
    // | designator
    // | call-expression
    // | literal .
    public void expression3()
    {

        if (accept(Token.Kind.NOT)) {
            expression3();
        } else if (accept(Token.Kind.OPEN_PAREN)) {
            expression0();
            expect(Token.Kind.CLOSE_PAREN); 
        } else if (have(Token.Kind.CALL)) {
            call_expression();
        } else if (have(NonTerminal.DESIGNATOR)) {
            designator();
        } else if (have(NonTerminal.LITERAL)) {
            literal();
        } else {
        	throw new QuitParseException(reportSyntaxError(NonTerminal.EXPRESSION3));
        }

    }
    
    // call-expression := "::" IDENTIFIER "(" expression-list ")" .
    public void call_expression() {

        expect(Token.Kind.CALL);
        // need to check if this function was previously declared 
        tryResolveSymbol(expectRetrieve(Token.Kind.IDENTIFIER));
        expect(Token.Kind.OPEN_PAREN);
        expression_list();
        expect(Token.Kind.CLOSE_PAREN);

    }
    
    // expression-list := [ expression0 { "," expression0 } ] .
    public void expression_list() {

        if (have(NonTerminal.EXPRESSION0)) {
            expression0();
            while (accept(Token.Kind.COMMA)) {
                expression0();
            }
        }

    }
    
    // parameter := IDENTIFIER ":" type .
    public void parameter() {
    	
    	tryDeclareSymbol(expectRetrieve(Token.Kind.IDENTIFIER));
        expect(Token.Kind.COLON);
        type();

    }
    
    
    // parameter-list := [ parameter { "," parameter } ] .
    public void parameter_list() {

        if (have(NonTerminal.PARAMETER)) {
            parameter();
            while (accept(Token.Kind.COMMA)) {
                parameter();
            }
        }
     
    }
    
    // variable-declaration := "var" IDENTIFIER ":" type ";"
    public void variable_declaration() {
    	
        expect(Token.Kind.VAR);
        // we now need to check if this identifier is in our symbol table
        tryDeclareSymbol(expectRetrieve(Token.Kind.IDENTIFIER));
        expect(Token.Kind.COLON);
        type();
        expect(Token.Kind.SEMICOLON);
    }
    
    // array-declaration := "array" IDENTIFIER ":" type "[" INTEGER "]" { "[" INTEGER "]" } ";"
    public void array_declaration() {

        expect(Token.Kind.ARRAY);
        expect(Token.Kind.IDENTIFIER);
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

    }
    
    // function-definition := "func" IDENTIFIER "(" parameter-list ")" ":" type statement-block .
    public void function_definition() {
    	

        expect(Token.Kind.FUNC);
        tryDeclareSymbol(expectRetrieve(Token.Kind.IDENTIFIER));
        expect(Token.Kind.OPEN_PAREN);
    	enterScope();
        // must determine a way to continue even if parameter-list is empty
        parameter_list();
        expect(Token.Kind.CLOSE_PAREN);
        expect(Token.Kind.COLON);
        // Next is 
        type();
        statement_block();  
        exitScope();

    }
    
    // declaration := variable-declaration | array-declaration | function-definition .
    public void declaration() {

        if (have(NonTerminal.VARIABLE_DECLARATION)) {
            variable_declaration();
        } else if (have(NonTerminal.ARRAY_DECLARATION)) {
            array_declaration();
        } else if (have(NonTerminal.FUNCTION_DEFINITION)) {
            function_definition();
        } else {
        	throw new QuitParseException(reportSyntaxError(NonTerminal.DECLARATION));
        }

    }
    
    // declaration-list := { declaration } .
    public void declaration_list() {

        while (have(NonTerminal.DECLARATION)) {
            declaration();
        }       

    }
    
    // assignment-statement := "let" designator "=" expression0 ";"
    public void assignment_statment() {

        expect(Token.Kind.LET);
    	tryResolveSymbol(currentToken);
        designator();
        expect(Token.Kind.ASSIGN);
        expression0();
        expect(Token.Kind.SEMICOLON);

    }
    
    // call-statement := call-expression ";"
    public void call_statement() {

        call_expression();
        expect(Token.Kind.SEMICOLON);       

    }
    
    // if-statement := "if" expression0 statement-block [ "else" statement-block ] .
    public void if_statement() {
        expect(Token.Kind.IF);
        expression0();
        // enter a new scope for the if statement
    	enterScope();        
        statement_block();
        exitScope();
        if (accept(Token.Kind.ELSE)) {
        	enterScope();
            statement_block();
            exitScope();
        }
    }
    
    // while-statement := "while" expression0 statement-block .
    public void while_statement() {

        expect(Token.Kind.WHILE);
        expression0();
    	enterScope();
        statement_block();   
        exitScope();

    }
    
    // return-statement := "return" expression0 ";" .
    public void return_statement() {

        expect(Token.Kind.RETURN);
        expression0();
        expect(Token.Kind.SEMICOLON);       

    }
    
    
    // statement := variable-declaration 
    // | call-statement  
    // | assignment-statement 
    // | if-statement 
    // | while-statement 
    // | return-statement .
    public void statement() {

        if (have(Token.Kind.VAR)) {
            variable_declaration();
        } else if (have(Token.Kind.CALL)) {
            call_statement();
        } else if (have(Token.Kind.LET)) {
            assignment_statment();
        } else if (have(Token.Kind.IF)){
            if_statement();
        } else if (have(Token.Kind.WHILE)) {
            while_statement();
        } else if (have(Token.Kind.RETURN)) {
            return_statement();
        } else {
        	throw new QuitParseException(reportSyntaxError(NonTerminal.STATEMENT));	
        }

    }
    
    // { statement } .
    public void statement_list() {

        
        while(have(NonTerminal.STATEMENT)) {
            statement();
        }

    }
    
    // "{" statement-list "}" .
    public void statement_block() {

        expect(Token.Kind.OPEN_BRACE);
        statement_list();
        expect(Token.Kind.CLOSE_BRACE);
        
    }
    
    // program := declaration-list EOF .
    public void program()
    {
        declaration_list();
        expect(Token.Kind.EOF);   
        
        // throw new RuntimeException("implement symbol table into grammar rules");
    }
    
}
