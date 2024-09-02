#ifndef MAGSTRUCTURE_H
#define MAGSTRUCTURE_H

struct MagStructure
{
    double operator [](int i) const {
        switch (i)
        {
        case 0:
            return x;
        
        case 1:
            return y;

        case 2:
            return z;
            
        default:
            abort();
        }
    }
    double & operator [](int i) {
        switch (i)
        {
        case 0:
            return x;
        
        case 1:
            return y;

        case 2:
            return z;

        default:
            abort();
        }
    }

    double x;
    double y;
    double z;
};

#endif