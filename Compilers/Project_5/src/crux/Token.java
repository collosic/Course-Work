package crux;

public class Token {
	
	public static enum Kind {
		AND("and"),
		OR("or"),
		NOT("not"),
		LET("let"),
		VAR("var"),
		ARRAY("array"),
		FUNC("func"),
		IF("if"),
		ELSE("else"),
		WHILE("while"),
		TRUE("true"),
		FALSE("false"),
		RETURN("return"),
		
		OPEN_PAREN("("),
		CLOSE_PAREN(")"),
		OPEN_BRACE("{"),
		CLOSE_BRACE("}"),
		OPEN_BRACKET("["),
		CLOSE_BRACKET("]"),		
		ADD("+"),
		SUB("-"),
		MUL("*"),
		DIV("/"),
		GREATER_EQUAL(">="),
		LESSER_EQUAL("<="),
		NOT_EQUAL("!="),
		EQUAL("=="),
		GREATER_THAN(">"),
		LESS_THAN("<"),
		ASSIGN("="),
		COMMA(","),
		SEMICOLON(";"),
		COLON(":"),
		CALL("::"),
		
		
		IDENTIFIER(),
		INTEGER(),
		FLOAT(),
		ERROR(),
		EOF();
		

		
		private String default_lexeme;
		
		Kind()
		{
			default_lexeme = "";
		}
		
		Kind(String lexeme)
		{
			default_lexeme = lexeme;
		}
		
		public boolean hasStaticLexeme()
		{
			return default_lexeme != null;
		}
		
		// OPTIONAL: if you wish to also make convenience functions, feel free
		//           for example, boolean matches(String lexeme)
		//           can report whether a Token.Kind has the given lexeme
		public boolean matches(String lexeme) 
		{
			return default_lexeme == lexeme;
		}
	}
	
	private int lineNum;
	private int charPos;
	Kind kind;
	private String lexeme = "";
	
	
	// OPTIONAL: implement factory functions for some tokens, as you see fit
         
	public static Token EOF(int linePos, int charPos)
	{
		Token tok = new Token(linePos, charPos);
		tok.kind = Kind.EOF;
		return tok;
	}

	public static Token INTEGER(String num, int linePos, int charPos)
	{
		Token tok = new Token(linePos, charPos);
		tok.kind = Kind.INTEGER;
		tok.lexeme = num;
		return tok;
	}
	
	public static Token FLOAT(String num, int linePos, int charPos)
	{
		Token tok = new Token(linePos, charPos);
		tok.kind = Kind.FLOAT;
		tok.lexeme = num;
		return tok;
	}
	
	public static Token IDENTIFIER(String name, int linePos, int charPos)
	{
		Token tok = new Token(linePos, charPos);
		tok.kind = Kind.IDENTIFIER;
		tok.lexeme = name;
		return tok;
	}
	
	private Token(int lineNum, int charPos)
	{
		this.lineNum = lineNum;
		this.charPos = charPos;
		
		// if we don't match anything, signal error
		this.kind = Kind.ERROR;
		this.lexeme = "No Lexeme Given";
	}
	
	public Token(String lexeme, int lineNum, int charPos)
	{
		this.lineNum = lineNum;
		this.charPos = charPos;
		
		// TODO: based on the given lexeme determine and set the actual kind

		for (Kind kinds : Kind.values()) {
			if(kinds.default_lexeme.equals(lexeme)) {
				this.kind = kinds;
				this.lexeme = kinds.default_lexeme;
				break;
			}
		}
		
		if (kind == null) {
			// if we don't match anything, signal error
			this.kind = Kind.ERROR;
			this.lexeme = "Unexpected character: " + lexeme;
		}
	}
	
	public int lineNumber()
	{
		return lineNum;
	}
	
	public int charPosition()
	{
		return charPos;
	}
	
	// Return the lexeme representing or held by this token
	public String lexeme()
	{
		// TODO: implement
		return lexeme;
	}
	
	public String toString()
	{
		// TODO: implement this
		String output;
		if (kind == Kind.ERROR || kind == Kind.INTEGER || kind == Kind.FLOAT || kind == Kind.IDENTIFIER) {
			output = kind + "(" + this.lexeme + ")" + "(lineNum:" + lineNum + ", charPos:" + charPos + ")";
		} else {
			output = kind + "(lineNum:" + lineNum + ", charPos:" + charPos + ")";
		}							
		return output;
	}
	
	// OPTIONAL: function to query a token about its kind
	//           boolean is(Token.Kind kind)
	public boolean is(Token.Kind kind) {
		boolean value = this.kind == kind;
		return value;
	}
	
	public Kind kind() {
		return this.kind;
	}

	// OPTIONAL: add any additional helper or convenience methods
	//           that you find make for a clean design

}
