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
            System.out.println("Error parsing file " + sourceFilename);
            System.out.println(p.errorReport());
            System.exit(-3);
        }
        
        ast.PrettyPrinter pp = new ast.PrettyPrinter();
        syntaxTree.accept(pp);
        System.out.println(pp.toString());
        output.println(pp.toString());
        output.close();
    }
}
    