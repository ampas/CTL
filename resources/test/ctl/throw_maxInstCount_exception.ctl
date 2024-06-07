const unsigned int loop_limit = UINT_MAX;

void main
    (input varying float rIn,
     input varying float gIn,
     input varying float bIn,
     output varying float rOut,
     output varying float gOut,
     output varying float bOut
     )
{
    unsigned int x = 0;
    for( unsigned int i = 0; i < loop_limit; i = i + 1)
    {
        x = i;
    }
    
    rOut = x;
	gOut = x;
	bOut = x;
}
