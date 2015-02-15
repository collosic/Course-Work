package crux;
import java.io.IOException;
import java.io.Reader;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import crux.Token.Kind;

public class Scanner implements Iterable<Token> {
	public static String studentName = "Christian Collosi";
	public static String studentID = "11233529";
	public static String uciNetID = "ccollosi@uci.edu";
	
	private final List<Token> tokens = new ArrayList<Token>();
	private int lineNum;  // current line count
	private int charPos;  // character offset for current line
	private int nextChar; // contains the next char (-1 == EOF)
	private Reader input;
	
	private final int NEWLINE = 10;
	private final int EQUALS = 61;
	private final int COLON = 58;
	private final int DECIMAL = 46;
	private final int EOF = -1;
	
	// Scanner Constructor
	Scanner(Reader reader) 
	{
		// TODO: initialize the Scanner
		input = reader;
		lineNum = 1;
		charPos = 0;
		nextChar = readChar();
	}	
	
	// OPTIONAL: helper function for reading a single char from input
	//           can be used to catch and handle any IOExceptions,
	//           advance the charPos or lineNum, etc.
	
	private int readChar() {
		int c = 0;
		try {
			c = input.read();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		charPos++;
		return c;
	}
	
	/* Invariants:
	 *  1. call assumes that nextChar is already holding an unread character
	 *  2. return leaves nextChar containing an untokenized character
	 */
	public Token next()
	{
		// TODO: implement this
		// Check for any type of white space
		while (Character.isWhitespace(nextChar)) {
			if(nextChar == EOF) break;
			// increment or reset lineNum and/or charPos
			clearDeadSpace();
		}
		String s = String.valueOf((char) nextChar);
		return (nextChar == EOF) ? Token.EOF(lineNum, charPos) : stateMachine(s);
	}

	// OPTIONAL: any other methods that you find convenient for implementation or testing
	private Token stateMachine(String s) {
		nextChar = readChar();
		
		// check and see if we have a comment and/or EOF
		do {
			s = checkForComments(s, nextChar);
		} while (s.equals("/") && nextChar == '/');

		if (s.equals("EOF")) return Token.EOF(lineNum, charPos);
		
		if (isSpecialCase(s)) {
			if ((s.equals(":") && nextChar == COLON) || (isComparator(s) && nextChar == EQUALS)) {
				nextChar = readChar();
				s += s.equals(":") ? ":" : "=";
				return new Token(s, lineNum, charPos - s.length());
			} else {
				return new Token(s, lineNum, charPos - s.length());	
			}
		} else if (isAlphaNumeric(s)) {
			return determineKind(s);	
		} else {
			return new Token(s, lineNum, charPos - s.length());
		}
	}
	
	private boolean isSpecialCase(String s) {
		if (isComparator(s) || s.equals(":")) return true;
		return false;
	}
	
	private boolean isComparator(String s) {
		if (s.equals("<") || s.equals(">") || s.equals("!") || s.equals("=") || s.equals(":")) {
			return true;
		} 	
		return false;
	}
	
	// includes underscore
	private boolean isAlphaNumeric(String s) {
		char[] c = s.toCharArray();
		return isLetter(c[0]) || isDigit(c[0]);
	}

	private boolean isLetter(char c) {
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
			return true;
		}
		return false;
	}
	
	private boolean isDigit(char c) {
		if (c >= '0' && c <= '9') {
			return true;
		}
		return false;
	}
	
	private String checkForComments(String s, int next) {
		if (s.equals("/") && (char) next == '/') {
			while (nextChar != NEWLINE) {
				nextChar = readChar();
				if (nextChar == EOF) break;
			}
			String left_over;
			if (nextChar != EOF) {
				clearDeadSpace();
				if (nextChar == EOF) {
					left_over = "EOF";
				} else {
					left_over = String.valueOf((char) nextChar);
					nextChar = readChar();
				}
			} else {
				left_over = "EOF";
			}
			
			return left_over;
		} else {
			return s;
		}
	}
	
	private void clearDeadSpace() {
		if (nextChar == NEWLINE) {
			while(nextChar == NEWLINE) {
				lineNum++;
				charPos = 0;
				nextChar = readChar();
			}
		}
		while(Character.isWhitespace(nextChar)) {
			if(nextChar == EOF || nextChar == NEWLINE) break;
			nextChar = readChar();
		}
	}

	private Token determineKind(String s) {
		char[] c = s.toCharArray();
		return isDigit(c[0]) ? extractNumber(s) : extractString(s);
	}
	
	private Token extractString(String s) {
		while (isLetter((char) nextChar) || isDigit((char) nextChar)) {
			s += (char) nextChar;
			nextChar = readChar();
		}
		Token t = new Token(s, lineNum, charPos - s.length());
		if (t.is(Kind.ERROR)) {
			t = Token.IDENTIFIER(s, lineNum, charPos - s.length());
		}
		return t;
	}
	
	private Token extractNumber(String s) {
		while (isDigit((char) nextChar) || nextChar == DECIMAL) {
			if (nextChar == DECIMAL) {
				s += (char) nextChar;
				nextChar = readChar();
				return extractFloat(s);
			}
			s += (char) nextChar;
			nextChar = readChar();
		}
		return Token.INTEGER(s, lineNum, charPos - s.length());
	}
	
	private Token extractFloat(String s) {
		while (isDigit((char) nextChar)) {
			s += (char) nextChar;
			nextChar = readChar();
		}
		return Token.FLOAT(s, lineNum, charPos - s.length());
	}
	
	// This is used to iterate through all the tokens in the arraylist
	@Override
	public Iterator<Token> iterator() {
		// TODO Auto-generated method stub
		return tokens.iterator();
	}
}
