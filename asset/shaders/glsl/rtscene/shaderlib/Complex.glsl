#ifndef Complex_H
#define Complex_H

struct Complex
{
    float re;
    float im;
};

Complex Add(Complex X, Complex Z)
{
    Complex result;
    result.re = X.re + Z.re;
    result.im = X.re + Z.im;
    return result;
}

Complex Sub(Complex X, Complex Z)
{
    Complex result;
    result.re = X.re - Z.re;
    result.im = X.re - Z.im;
    return result;
}

Complex Mul(Complex X, Complex Z)
{
    Complex result;
    result.re = X.re * Z.re - X.im * Z.im;
    result.im = X.re * Z.im + X.im * Z.re;
    return result;
}

Complex Div(Complex X, Complex Z)
{
    float scale = 1 / (Z.re * Z.re + Z.im * Z.im);

    Complex result;
    result.re = scale * (Z.re * Z.re + X.im * Z.im);
    result.im = scale * (Z.im * Z.re - X.re * Z.im);
    return result;
}

Complex Add(float value, Complex Z)
{
    Complex X;
    X.re = value;
    X.im = 0;
    return Add(X, Z);
}

Complex Sub(float value, Complex Z)
{
    Complex X;
    X.re = value;
    X.im = 0;
    return Sub(X, Z);
}

Complex Mul(float value, Complex Z)
{
    Complex X;
    X.re = value;
    X.im = 0;
    return Mul(X, Z);
}

Complex Div(float value, Complex Z)
{
    Complex X;
    X.re = value;
    X.im = 0;
    return Div(X, Z);
}

Complex Sqr(Complex z)
{
    return Mul(z, z);
}


float norm(Complex z) 
{
    return z.re * z.re + z.im * z.im;
}


float abs_Complex(Complex z)
{
    return sqrt(norm(z));
}

Complex sqrt_Complex(Complex z)
{
    float n = abs_Complex(z);
    float t1 = sqrt( (0.5 * (n + abs(z.re))) );
    float t2 = 0.5 * z.im / t1;

    if (n == 0)
        return Complex(0, 0);

    if (z.re >= 0)
        return Complex(t1, t2);
    else
        return Complex( abs(t2), Copysign(t1, z.im) );
}


#endif