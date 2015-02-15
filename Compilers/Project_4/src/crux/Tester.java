package crux;
import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.io.FileWriter;
import java.util.LinkedList;
import java.util.List;
import java.io.PrintWriter;

public class Tester {
	
	public static final int PASS = 0;
	public static final int FAIL = 1;
	public static final int NOT_ACCESSABLE = 2;
	public static final int IO_ERROR = 3;
	public static final int SLEEP_ERROR = 4;
	public static final String SCORES_TXT = "../scores_project4.csv";

	public static int testPublic(int testNum) {
		String inputFilename = String.format("tests/test%02d.crx", testNum);
		String outputFilename = String.format("tests/test%02d.rea", testNum);
		String expectedFilename = String.format("tests/test%02d.out", testNum);

		Scanner s = null;
		try {
			s = new Scanner(new FileReader(inputFilename));
		} catch (IOException e) {
			e.printStackTrace();
			return NOT_ACCESSABLE;
		}

		try {
			PrintStream outputStream = new PrintStream(outputFilename);
			// Parser p = new Parser(s);
// 			p.parse();
			
			Parser p = new Parser(s);
	        ast.Command syntaxTree = p.parse();
			
			
			if (p.hasError()) {
				outputStream.println("Error parsing file.");
				outputStream.println(p.errorReport());
				outputStream.close();
			}
			
            ast.PrettyPrinter pp = new ast.PrettyPrinter();
            syntaxTree.accept(pp);
            outputStream.println(pp.toString());
			
			// outputStream.println("Crux program successfully parsed.");
			outputStream.close();

		} catch (IOException e) {
			System.err.println("Error opening output file: \"" + outputFilename + "\"");
			e.printStackTrace();
			return IO_ERROR;
		}

		BufferedReader bufferedexpected;
		BufferedReader bufferedoutput;

		String lineExpected;
		String lineOutput;

		try {
			bufferedexpected = new BufferedReader(new FileReader(expectedFilename));
			bufferedoutput = new BufferedReader(new FileReader(outputFilename));
		} catch (FileNotFoundException e) {
			e.printStackTrace();
			return IO_ERROR;
		}
		int result = PASS;

		try {
			while ((lineExpected = bufferedexpected.readLine()) != null) {
				lineOutput = bufferedoutput.readLine();
				if (lineOutput == null) {
					result = FAIL;
					break;
				}
				lineExpected = lineExpected.replaceAll("\\s+$", "");
				lineOutput = lineOutput.replaceAll("\\s+$", "");
				if (!lineExpected.equals(lineOutput)) {
					result = FAIL;
					break;
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
			result = IO_ERROR;
		}

		try {
			bufferedoutput.close();
			bufferedexpected.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return result;
	}
	
	public static int calScore(int passNum) {
		int base = 70;
		int score = passNum * 3 + base;
		return score;
	}

	public static void main(String args[]) throws IOException {

		// String studentName = Compiler.studentName;
		String studentID = Compiler.studentID;
		String uciNetID = Compiler.uciNetID;

		int publicTestcaseNum = 10;

		int publicPass = 0;
		for (int i = 1; i <= publicTestcaseNum; ++i) {
			if (testPublic(i) == PASS) {
				++publicPass;
			}
		}
		int score = calScore(publicPass);
		
		System.out.print(studentID);
		System.out.print("\t");
		System.out.print(uciNetID);
		System.out.print("\t");
		System.out.print("score: " + score);
		System.out.print("\t");
		System.out.print(" Passed Public Cases: ");
		System.out.print(publicPass);
		System.out.print("/");
		System.out.println(publicTestcaseNum);
		
		// record the scores
		try {
			PrintWriter scoreStream = new PrintWriter(new FileWriter(SCORES_TXT,true));
			scoreStream.print(studentID);
			scoreStream.print(",");
			
			// int score = calScore(publicPass);
			scoreStream.print(score);
			scoreStream.print(",");
			
			scoreStream.print("Passed Public Cases: ");
			scoreStream.print(publicPass);
			scoreStream.print("/");
			scoreStream.println(publicTestcaseNum);
			
			scoreStream.close();
			System.out.println("finish recording score");

		} catch (IOException e) {
			System.err.println("Error opening output file: \"" + SCORES_TXT + "\"");
			e.printStackTrace();
		}
		
	}
}
