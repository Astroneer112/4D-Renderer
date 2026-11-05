#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

class vec4{
public:
    float x;
    float y;
    float z;
    float w;
    vec4(){
        x = 0;
        y = 0;
        z = 0;
        w = 0;
    }
    vec4(float x_, float y_, float z_, float w_){
        x = x_;
        y = y_;
        z = z_;
        w = w_;
    }

    vec4 operator+(const vec4& o) const {
        return vec4(x+o.x, y+o.y, z+o.z, w+o.w);
    }
    vec4 operator-(const vec4& o) const {
        return vec4(x-o.x, y-o.y, z-o.z, w-o.w);
    }
    vec4 operator*(const vec4& o) const {
        return vec4(x*o.x, y*o.y, z*o.z, w*o.w);
    }
    vec4 operator/(const vec4& o) const {
        return vec4(x/o.x, y/o.y, z/o.z, w/o.w);
    }

    vec4 operator+(const float& o) const {
        return vec4(x+o, y+o, z+o, w+o);
    }
    vec4 operator-(const float& o) const {
        return vec4(x-o, y-o, z-o, w-o);
    }
    vec4 operator*(const float& o) const {
        return vec4(x*o, y*o, z*o, w*o);
    }
    vec4 operator/(const float& o) const {
        return vec4(x/o, y/o, z/o, w/o);
    }

    vec4 sub_before(const float& o) const{
        return vec4(o-x, o-y, o-z, o-w);
    }

    float mag() const{
        return sqrt(x*x + y*y + z*z + w*w);
    }
    vec4 min(const vec4& o) const{
        return vec4(std::min(x, o.x), std::min(y, o.y), std::min(z, o.z), std::min(w, o.w));
    }
    vec4 min(const float o) const{
        return vec4(std::min(x, o), std::min(y, o), std::min(z, o), std::min(w, o));
    }
    vec4 max(const vec4& o) const{
        return vec4(std::max(x, o.x), std::max(y, o.y), std::max(z, o.z), std::max(w, o.w));
    }
    vec4 max(const float o) const{
        return vec4(std::max(x, o), std::max(y, o), std::max(z, o), std::max(w, o));
    }

    vec4 normalize() const{
        float m = mag();
        return vec4(x/m, y/m, z/m, w/m);
    }

    float dot(const vec4 o) const{
        return x * o.x + y * o.y + z * o.z + w * o.w;
    }
};

class vec3{
public:
    float x;
    float y;
    float z;
    vec3(){
        x = 0;
        y = 0;
        z = 0;
    }
    vec3(float x_, float y_, float z_){
        x = x_;
        y = y_;
        z = z_;
    }
    vec3(vec4 a){
        x = a.x;
        y = a.y;
        z = a.z;
    }
    vec3 operator+(const vec3& o) const {
        return vec3(x+o.x, y+o.y, z+o.z);
    }
    vec3 operator-(const vec3& o) const {
        return vec3(x-o.x, y-o.y, z-o.z);
    }
    vec3 operator*(const vec3& o) const {
        return vec3(x*o.x, y*o.y, z*o.z);
    }
    vec3 operator/(const vec3& o) const {
        return vec3(x/o.x, y/o.y, z/o.z);
    }

    vec3 operator+(const float& o) const {
        return vec3(x+o, y+o, z+o);
    }
    vec3 operator-(const float& o) const {
        return vec3(x-o, y-o, z-o);
    }
    vec3 operator*(const float& o) const {
        return vec3(x*o, y*o, z*o);
    }
    vec3 operator/(const float& o) const {
        return vec3(x/o, y/o, z/o);
    }

    float mag() const{
        return sqrt(x*x + y*y + z*z);
    }
};

class vec2{
public:
    float x;
    float y;
    vec2(){
        x = 0;
        y = 0;
    }
    vec2(float x_, float y_){
        x = x_;
        y = y_;
    }
    vec2 max(const float o) const{
        return vec2(std::max(x, o), std::max(y, o));
    }
    float mag() const{
        return sqrt(x*x + y*y);
    }
};

class sdf_return{
public:
    float dist;
    uint32_t color;
    float spaceDensity;
    vec4 densityDir;

    sdf_return(){
        dist = -1.0f;
        color = 0xFFFFFFFF;
        spaceDensity = 1;
        densityDir = vec4();
    }
    sdf_return(float dist_){
        dist = dist_;
        color = 0xFFFFFFFF;
        spaceDensity = 1;
        densityDir = vec4();
    }
    sdf_return(float dist_, uint32_t color_){
        dist = dist_;
        color = color_;
        spaceDensity = 1;
        densityDir = vec4();
    }
    sdf_return(float dist_, uint32_t color_, float spaceDensity_, vec4 densityDir_){
        dist = dist_;
        color = color_;
        spaceDensity = spaceDensity_;
        densityDir = densityDir_;
    }

    sdf_return min(const sdf_return o) const{
        if(dist < o.dist){
            return *this;
        } else{
            return o;
        }
    }
    sdf_return max(const sdf_return o) const{
        if(dist > o.dist){
            return *this;
        } else{
            return o;
        }
    }
    sdf_return max(float o){
        return sdf_return(std::max(dist, o), color, spaceDensity, densityDir);
    }

    sdf_return operator-(){
        return sdf_return(-dist, color, spaceDensity, densityDir);
    }

    sdf_return operator-(float o){
        return sdf_return(dist - o, color, spaceDensity, densityDir);
    }
};

class sdf{
public:
    float sharpness = 0.2f;
    float throatLength = 2;
    float wormholeRad = 0.5f;
    vec4 wormholePos = vec4(0,1,0,throatLength/2.0f);

    float spaceRad = 10.0f;

    struct Frame{vec4 x, y, z;};

    sdf(){}

    Frame tangentFrame(vec4 p){
        vec4 n = spaceNorm(p);
        vec4 axes[4] = { vec4(1,0,0,0), vec4(0,1,0,0), vec4(0,0,1,0), vec4(0,0,0,1) };
        vec4 out[3]; int k = 0;
        for(int i = 0; i < 4 && k < 3; ++i){
            vec4 v = axes[i] - n * axes[i].dot(n);
            for(int j = 0; j < k; ++j) v = v - out[j] * v.dot(out[j]);
            if(v.mag() > 0.1f) out[k++] = v.normalize();
        }
        return Frame{ out[0], out[1], out[2] };
    }

    vec4 rotAB(const vec4& v, const vec4& a, const vec4& b){
        float c = a.dot(b);
        vec4 w = b - a * c;
        float s = w.mag();
        if(s < 1e-7f) return v;
        w = w / s;
        float va = v.dot(a), vw = v.dot(w);
        return v + a * ((c - 1.0f) * va - s * vw) + w * (s * va + (c - 1.0f) * vw);
    }

    vec4 projectToSpace(vec4 p){
        for(int i = 0; i < 5; ++i){
            float d = space_sdf(p).dist;
            if(std::abs(d) < 1e-5f) break;
            vec4 n = spaceNorm(p);
            if(n.mag() == 0.0f) break;
            p = p - n * d;
        }
        return p;
    }

    vec4 localToWorld(vec4 origin, vec3 local, const Frame& f, float maxStep = 0.01f){
        vec4 p = projectToSpace(origin);
        float len = local.mag();
        if(len < 1e-7f) return p;

        vec4 dir = (f.x * local.x + f.y * local.y + f.z * local.z) / len;

        for(float moved = 0; moved < len; ){
            vec4 n = spaceNorm(p);
            if(n.mag() > 0.0f){
                vec4 t = dir - n * dir.dot(n);
                if(t.mag() > 1e-6f) dir = t.normalize();
            }
            float step = std::min(maxStep, len - moved);
            p = p + dir * step;
            moved += step;
            p = projectToSpace(p);
        }
        return p;
    }

    vec4 feltToPhys(const sdf_return& s, vec4 dir){
        if(s.densityDir.mag() == 0.0f) return dir / s.spaceDensity;    // isotropic
        float a = dir.dot(s.densityDir);
        return dir + s.densityDir * (a * (1.0f / s.spaceDensity - 1.0f));
    }

    sdf_return sdf_sphere_4d(vec4 p, vec4 c, float r){
        return (p - c).mag() - r;
    }
    sdf_return sdf_extrude_sphere(vec4 p, vec4 c, float r, float h){
        vec3 pDist = vec3(p.x - c.x, p.y - c.y, p.z - c.z);
        float d = pDist.mag() - r;

        float dw = std::abs(p.w - c.w) - h / 2.0f;

        float ext_d = std::max(d, 0.0f);
        float ext_dw = std::max(dw, 0.0f);

        return std::min(std::max(d, dw), 0.0f) + sqrt(ext_d * ext_d + ext_dw * ext_dw);
    }
    sdf_return sdf_arc_tube_xw(vec4 p, vec4 start, vec4 end, bool dir, float r, float density){
        vec4  c   = (start + end) * 0.5f;
        vec2  d   = vec2(end.x - start.x, end.w - start.w);
        float rho = d.mag() * 0.5f;
        vec2  side = dir ? vec2(d.y, -d.x) : vec2(-d.y, d.x);
        float qx = p.x - c.x, qw = p.w - c.w;
        float d_tube;

        vec4 t;
        float u0x = (start.x - c.x) / rho, u0w = (start.w - c.w) / rho;
        float cr = u0x * qw - u0w * qx, dt = u0x * qx + u0w * qw;
        float theta = std::atan2(dir ? cr : -cr, dt);          // 0 at start .. pi at end
        float sArc = rho * theta, L = rho * (float)M_PI;

        auto smooth = [](float a, float b, float x){ float h = std::clamp((x - a) / (b - a), 0.0f, 1.0f); return h*h*(3 - 2*h); };
        float e0 = r + sharpness, e1 = e0 + 1.0f;               // stay at 1 through the mouth + fillet, ramp over 1 unit
        float ramp = smooth(e0, e1, sArc) * smooth(e0, e1, L - sArc);
        if(qx * side.x + qw * side.y >= 0.0f){
            float qm = vec2(qx, qw).mag();
            float inPlane = vec2(qx, qw).mag() - rho;
            float perp2   = (p.y - c.y)*(p.y - c.y) + (p.z - c.z)*(p.z - c.z);
            d_tube = std::sqrt(inPlane*inPlane + perp2);
            if(qm > 1e-6f) t = vec4(-qw / qm, 0, 0, qx / qm);
        } else{
            d_tube = std::min((p - start).mag(), (p - end).mag());
            float sm = side.mag();
            vec4 tS = vec4(side.x / sm, 0, 0, side.y / sm);
            t = ((p - start).mag() < (p - end).mag()) ? tS : tS * -1.0f;
        }
        sdf_return out(d_tube - r);
        out.densityDir = t;
        out.spaceDensity = 1.0f + (density - 1.0f) * ramp;
        return out;
    }

    sdf_return sdf_hypercylinder_xw(vec4 p, vec4 c, float r){
        return vec2(p.x - c.x, p.w - c.w).mag() - r;
    }

    sdf_return sdf_plane_2d(vec4 p, vec4 c, vec4 n1, vec4 n2){
        return sdf_return(sqrt((p-c).dot(n1) * (p-c).dot(n1) + (p-c).dot(n2) * (p-c).dot(n2)));
    }

    sdf_return union_round(sdf_return a, sdf_return b, float r){
        vec2 q = vec2(r - a.dist, r - b.dist).max(0.0f);
        sdf_return closer = a.min(b);
        float final_dist = std::max(closer.dist, r) - q.mag();
        return sdf_return(final_dist, closer.color, closer.spaceDensity, closer.densityDir);
    }

    sdf_return sdf_wormholes(vec4 p){
        sdf_return wormhole0 = sdf_extrude_sphere(p, wormholePos, wormholeRad, throatLength);
        sdf_return wormhole1 = sdf_arc_tube_xw(p, vec4(10, 1, 0, -10), vec4(10, 1, 0, throatLength * 2 + spaceRad * 3), true, wormholeRad, 1.0f/66.0f);
        return wormhole0.min(wormhole1);
    }

    sdf_return space_sdf(vec4 p){
        sdf_return space0 = sdf_hypercylinder_xw(p, vec4(0,0,0,-spaceRad), spaceRad);
        space0.color = 0xFFFF0000;
        sdf_return space1 = sdf_hypercylinder_xw(p, vec4(0,0,0,throatLength + spaceRad), spaceRad);
        space1.color = 0xFF00FF00;
        sdf_return space2 = sdf_hypercylinder_xw(p, vec4(0,0,0,throatLength * 2 + spaceRad * 3), spaceRad);
        space2.color = 0xFF0000FF;


        sdf_return a = space0.min(space1).min(space2);
        sdf_return b = sdf_wormholes(p);

        return union_round(a,b,sharpness);
    }

    sdf_return scene_sdf(vec4 p){
        sdf_return floor = p.y;
        floor.color = space_sdf(p).color;

        return floor;

    }

    vec4 spaceNorm(vec4 p){
        float eps = 0.005f;

        // Differences: f(p + eps) - f(p - eps)
        float dx = space_sdf(p + vec4(eps, 0, 0, 0)).dist - space_sdf(p - vec4(eps, 0, 0, 0)).dist;
        float dy = space_sdf(p + vec4(0, eps, 0, 0)).dist - space_sdf(p - vec4(0, eps, 0, 0)).dist;
        float dz = space_sdf(p + vec4(0, 0, eps, 0)).dist - space_sdf(p - vec4(0, 0, eps, 0)).dist;
        float dw = space_sdf(p + vec4(0, 0, 0, eps)).dist - space_sdf(p - vec4(0, 0, 0, eps)).dist;

        vec4 g(dx, dy, dz, dw);
        float m = g.mag();
        if (m < 1e-8f) return vec4(0,0,0,0);
        return g / m;

    }

    sdf_return raymarch(vec4 p, vec4 dir){
        float dist = scene_sdf(p).dist;
        sdf_return moved = sdf_return(0);
        int minCounter = 0;

        while(dist > 0.001){
            if(moved.dist >= 10){
                return -1;
            }

            sdf_return throat = sdf_wormholes(p);

            float minStep = 0.1f * wormholeRad;
            float step = std::min(dist, std::max(throat.dist - sharpness, minStep));

            sdf_return here = space_sdf(p);
            vec4  v = feltToPhys(here, dir);
            float f = step / v.mag();
            p = p + v * f;
            moved.dist += f;

            if(step == minStep){
                minCounter += 1;
                if(minCounter >= 2000){
                    return -1;
                }
            } else{
                minCounter = 0;
            }

            p = projectToSpace(p);

            sdf_return there = space_sdf(p);
            vec4 t0 = here.densityDir, t1 = there.densityDir;
            if(here.spaceDensity < 1.0f && t0.mag() > 0.0f && t1.mag() > 0.0f){
                if(t0.dot(t1) < 0.0f) t1 = t1 * -1.0f;      // guard against a sign flip
                dir = rotAB(dir, t0, t1);                    // carry dir around the bend
            }

            vec4 n = spaceNorm(p);
            if (n.mag() > 0.0f) {
                vec4 t = dir - n * dir.dot(n);
                if (t.mag() > 1e-6f) dir = t.normalize();
            }

            dist = scene_sdf(p).dist;
        }
        moved.color = scene_sdf(p).color;
        return moved;
    }

    sdf_return raymarch_space(vec4 p, vec4 dir){
        float dist = space_sdf(p).dist;
        sdf_return moved = sdf_return(0);

        while(dist > 0.01){
            if(moved.dist >= 50){
                return sdf_return(-1);
            }
            p = p + dir * dist;
            moved.dist += dist;
            dist = space_sdf(p).dist;
        }
        moved.color = space_sdf(p).color;
        return moved;
    }
};
