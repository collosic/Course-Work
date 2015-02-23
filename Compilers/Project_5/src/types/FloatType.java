package types;

public class FloatType extends Type {
    
    public FloatType()
    {
        
    }
    
    @Override
    public String toString()
    {
        return "float";
    }
       
    @Override
    public boolean equivalent(Type that)
    {
        if (that == null)
            return false;
        if (!(that instanceof FloatType))
            return false;
        return true;
    }
}
