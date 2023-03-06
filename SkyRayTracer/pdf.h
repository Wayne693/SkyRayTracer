#pragma once
#include "vec3.h"
#include "math.h"
#include "onb.h"

class pdf
{
public:
	
	virtual float value(const vec3& direction) const = 0;
	virtual vec3 generate() const = 0;
};

class CosPDF : public pdf {
public:
    CosPDF(const vec3& w) { uvw.build_from_w(w); }

    virtual float value(const vec3& direction) const override {
        auto cosine = dot(direction.normalized(), uvw.w());
        return (cosine <= 0) ? 0 : cosine / PI;
    }

    virtual vec3 generate() const override {
        return uvw.local(random_cosine_direction());
    }

public:
    onb uvw;
};

class HittablePDF : public pdf
{
public:
    HittablePDF(std::shared_ptr<Hittable> p, const point3& origin):ptr(p),o(origin){}

    virtual float value(const vec3& direction) const override
    {
        return ptr->pdf_value(o, direction);
    }

    virtual vec3 generate() const override
    {
        return ptr->random(o);
    }

public:
    point3 o;
    std::shared_ptr<Hittable> ptr;
};

class MixPDF : public pdf {
public:
    MixPDF(std::shared_ptr<pdf> p0, std::shared_ptr<pdf> p1) {
        p[0] = p0;
        p[1] = p1;
    }

    virtual float value(const vec3& direction) const override {
        return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
    }

    virtual vec3 generate() const override {
        if (Random() < 0.5)
            return p[0]->generate();
        else
            return p[1]->generate();
    }

public:
    std::shared_ptr<pdf> p[2];
};