---
permalink: /docs/api-c/
redirect_from: /docs/index.html
---

## **C version**
The C API requires the [Lapack libraries](http://www.netlib.org/lapack/) which can be install with the following command:

```
sudo apt-get install liblapack-dev
```  

This version can be included in your code as follow:  
```c
//Include library
#include "Asali.h"

//Main
int main()
{
    //Create composition using Asali vectors format
    AsaliVector x,names;
    double X[3] = {0.1, 0.2, 0.7};
    empty_vector_of_char(&names,3);
    set_vector_element_from_char(&names,0,"H2");
    set_vector_element_from_char(&names,1,"O2");
    set_vector_element_from_char(&names,2,"N2");
    vector_from_double_array(&x,3,X);

    //Initialize Asali
    Asali asali;
    initialize(&asali);

    //Set up composition/pressure and temperature
    set_temperature(&asali,393.15);
    set_pressure(&asali,4e05);
    set_number_of_species(&asali,get_vector_size(&x));
    set_species_names(&asali,names);
    set_mole_fraction(&asali,x);

    //Properties evaluation
    AsaliVector cp;
    AsaliMatrix diff;

    diff = get_binary_diffusion(&asali);
    cp = get_species_mass_specific_heat(&asali);
    return 0;
}
```
Example and database generator can be compiled by typing:
```
./compile.sh
```

To convert the Asali database into C code run:
```
./database-generator
```