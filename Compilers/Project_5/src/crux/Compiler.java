package crux;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintStream;
import java.io.PrintWriter;

public class Compiler {
    public static String studentName = "Christian Collosi";
    public static String studentID = "11233529";
    public static String uciNetID = "ccollosi@uci.edu";
    
    public static void main(String[] args)
    {
        String sourceFilename = args[0];
        PrintWriter output = null;
        
        Scanner s = null;
        try {
            s = new Scanner(new FileReader(sourceFilename));
            output = new PrintWriter(new FileWriter("out.txt"));
        } catch (IOException e) {
            e.printStackTrace();
            System.err.println("Error accessing the source file: \"" + sourceFilename + "\"");
            System.exit(-2);
        }

        Parser p = new Parser(s);
        ast.Command syntaxTree = p.parse();
        if (p.hasError()) {
            System.out.println("Error parsing file.");
            System.out.println(p.errorReport());
            System.exit(-3);
        }
            
        types.TypeChecker tc = new types.TypeChecker();
        tc.check(syntaxTree);
        if (tc.hasError()) {
            System.out.println("Error type-checking file.");
            output.println("Error type-checking file.");
            System.out.println(tc.errorReport());
            output.println(tc.errorReport());
            output.close();
            System.exit(-4);
        }
        System.out.println("Crux Program has no type errors.");
        output.println("Crux Program has no type errors.");
        output.close();
    }
}
    
