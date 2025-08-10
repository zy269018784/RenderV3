#pragma once
#include <cpu_gpu.h>
#include <Math/Float.h>
#include <Math/Util.h>
#include <cmath>
namespace Render {
    // Interval Definition
    class Interval {
    public:
        // Interval Public Methods
        Interval() = default;
        CPU_GPU
            explicit Interval(Float v) : low(v), high(v) {}
        CPU_GPU constexpr Interval(Float low, Float high)
            : low(std::min(low, high)), high(std::max(low, high)) {}

        CPU_GPU
            static Interval FromValueAndError(Float v, Float err) {
            Interval i;
            if (err == 0)
                i.low = i.high = v;
            else {
                i.low = SubRoundDown(v, err);
                i.high = AddRoundUp(v, err);
            }
            return i;
        }

        CPU_GPU
            Interval& operator=(Float v) {
            low = high = v;
            return *this;
        }

        CPU_GPU
            Float UpperBound() const { return high; }
        CPU_GPU
            Float LowerBound() const { return low; }
        CPU_GPU
            Float Midpoint() const { return (low + high) / 2; }
        CPU_GPU
            Float Width() const { return high - low; }

        CPU_GPU
            Float operator[](int i) const {
            //DCHECK(i == 0 || i == 1);
            return (i == 0) ? low : high;
        }

        CPU_GPU
            explicit operator Float() const { return Midpoint(); }

        CPU_GPU
            bool Exactly(Float v) const { return low == v && high == v; }

        CPU_GPU
            bool operator==(Float v) const { return Exactly(v); }

        CPU_GPU
            Interval operator-() const { return { -high, -low }; }

        CPU_GPU
            Interval operator+(Interval i) const {
            return { AddRoundDown(low, i.low), AddRoundUp(high, i.high) };
        }

        CPU_GPU
            Interval operator-(Interval i) const {
            return { SubRoundDown(low, i.high), SubRoundUp(high, i.low) };
        }

        CPU_GPU
            Interval operator*(Interval i) const {
            Float lp[4] = { MulRoundDown(low, i.low), MulRoundDown(high, i.low),
                           MulRoundDown(low, i.high), MulRoundDown(high, i.high) };
            Float hp[4] = { MulRoundUp(low, i.low), MulRoundUp(high, i.low),
                           MulRoundUp(low, i.high), MulRoundUp(high, i.high) };
            return { std::min({lp[0], lp[1], lp[2], lp[3]}),
                    std::max({hp[0], hp[1], hp[2], hp[3]}) };
        }

        CPU_GPU
            Interval operator/(Interval i) const;

        CPU_GPU bool operator==(Interval i) const {
            return low == i.low && high == i.high;
        }

        CPU_GPU
            bool operator!=(Float f) const { return f < low || f > high; }

        // std::string ToString() const;

        CPU_GPU Interval& operator+=(Interval i) {
            *this = Interval(*this + i);
            return *this;
        }
        CPU_GPU Interval& operator-=(Interval i) {
            *this = Interval(*this - i);
            return *this;
        }
        CPU_GPU Interval& operator*=(Interval i) {
            *this = Interval(*this * i);
            return *this;
        }
        CPU_GPU Interval& operator/=(Interval i) {
            *this = Interval(*this / i);
            return *this;
        }
        CPU_GPU
            Interval& operator+=(Float f) { return *this += Interval(f); }
        CPU_GPU
            Interval& operator-=(Float f) { return *this -= Interval(f); }
        CPU_GPU
            Interval& operator*=(Float f) {
            if (f > 0)
                *this = Interval(MulRoundDown(f, low), MulRoundUp(f, high));
            else
                *this = Interval(MulRoundDown(f, high), MulRoundUp(f, low));
            return *this;
        }
        CPU_GPU
            Interval& operator/=(Float f) {
            if (f > 0)
                *this = Interval(DivRoundDown(low, f), DivRoundUp(high, f));
            else
                *this = Interval(DivRoundDown(high, f), DivRoundUp(low, f));
            return *this;
        }

#ifndef RENDER_IS_GPU_CODE
        static const Interval Pii;
#endif

    private:
        //friend struct SOA<Interval>;
        // Interval Private Members
        Float low, high;
    };

    CPU_GPU inline bool InRange(Float v, Interval i) {
        return v >= i.LowerBound() && v <= i.UpperBound();
    }

    inline Interval Interval::operator/(Interval i) const {
        if (InRange(0, i))
            // The interval we're dividing by straddles zero, so just
            // return an interval of everything.
            return Interval(-Infinity, Infinity);

        Float lowQuot[4] = { DivRoundDown(low, i.low), DivRoundDown(high, i.low),
                            DivRoundDown(low, i.high), DivRoundDown(high, i.high) };
        Float highQuot[4] = { DivRoundUp(low, i.low), DivRoundUp(high, i.low),
                             DivRoundUp(low, i.high), DivRoundUp(high, i.high) };
        return { std::min({lowQuot[0], lowQuot[1], lowQuot[2], lowQuot[3]}),
                std::max({highQuot[0], highQuot[1], highQuot[2], highQuot[3]}) };
    }

    CPU_GPU inline Interval Sin(Interval i) {
        //CHECK_GE(i.LowerBound(), -1e-16);
        //CHECK_LE(i.UpperBound(), 2.0001 * Pi);
        Float low = std::sin(std::max<Float>(0, i.LowerBound()));
        Float high = std::sin(i.UpperBound());
        if (low > high)
            Swap(low, high);
        low = std::max<Float>(-1, NextFloatDown(low));
        high = std::min<Float>(1, NextFloatUp(high));
        if (InRange(Pi / 2, i))
            high = 1;
        if (InRange((3.f / 2.f) * Pi, i))
            low = -1;

        return Interval(low, high);
    }

    CPU_GPU inline Interval Cos(Interval i) {
        //CHECK_GE(i.LowerBound(), -1e-16);
        //CHECK_LE(i.UpperBound(), 2.0001 * Pi);
        Float low = std::cos(std::max<Float>(0, i.LowerBound()));
        Float high = std::cos(i.UpperBound());
        if (low > high)
            Swap(low, high);
        low = std::max<Float>(-1, NextFloatDown(low));
        high = std::min<Float>(1, NextFloatUp(high));
        if (InRange(Pi, i))
            low = -1;

        return Interval(low, high);
    }


    CPU_GPU inline Interval operator+(Float f, Interval i) {
        return Interval(f) + i;
    }

    CPU_GPU inline Interval operator-(Float f, Interval i) {
        return Interval(f) - i;
    }

    CPU_GPU inline Interval operator*(Float f, Interval i) {
        if (f > 0)
            return Interval(MulRoundDown(f, i.LowerBound()), MulRoundUp(f, i.UpperBound()));
        else
            return Interval(MulRoundDown(f, i.UpperBound()), MulRoundUp(f, i.LowerBound()));
    }

    CPU_GPU inline Interval operator/(Float f, Interval i) {
        if (InRange(0, i))
            // The interval we're dividing by straddles zero, so just
            // return an interval of everything.
            return Interval(-Infinity, Infinity);

        if (f > 0)
            return Interval(DivRoundDown(f, i.UpperBound()), DivRoundUp(f, i.LowerBound()));
        else
            return Interval(DivRoundDown(f, i.LowerBound()), DivRoundUp(f, i.UpperBound()));
    }

    CPU_GPU inline Interval operator+(Interval i, Float f) {
        return i + Interval(f);
    }

    CPU_GPU inline Interval operator-(Interval i, Float f) {
        return i - Interval(f);
    }

    CPU_GPU inline Interval operator*(Interval i, Float f) {
        if (f > 0)
            return Interval(MulRoundDown(f, i.LowerBound()), MulRoundUp(f, i.UpperBound()));
        else
            return Interval(MulRoundDown(f, i.UpperBound()), MulRoundUp(f, i.LowerBound()));
    }

    CPU_GPU inline Interval operator/(Interval i, Float f) {
        if (f == 0)
            return Interval(-Infinity, Infinity);

        if (f > 0)
            return Interval(DivRoundDown(i.LowerBound(), f), DivRoundUp(i.UpperBound(), f));
        else
            return Interval(DivRoundDown(i.UpperBound(), f), DivRoundUp(i.LowerBound(), f));
    }

    

    //CPU_GPU inline Interval Sqr(Interval i) {
    //    Float alow = std::abs(i.LowerBound()), ahigh = std::abs(i.UpperBound());
    //    if (alow > ahigh)
    //        Swap(alow, ahigh);
    //    if (InRange(0, i))
    //        return Interval(0, MulRoundUp(ahigh, ahigh));
    //    return Interval(MulRoundDown(alow, alow), MulRoundUp(ahigh, ahigh));
    //}
    //
    //CPU_GPU inline Interval Sqrt(Interval i) {
    //    return { SqrtRoundDown(i.LowerBound()), SqrtRoundUp(i.UpperBound()) };
    //}
}