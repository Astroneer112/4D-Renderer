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

    sdf_return(){
        dist = -1.0f;
        color = 0xFFFFFFFF;
    }
    sdf_return(float dist_){
        dist = dist_;
        color = 0xFFFFFFFF;
    }
    sdf_return(float dist_, uint32_t color_){
        dist = dist_;
        color = color_;
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
        return sdf_return(std::max(dist, o), color);
    }

    sdf_return operator-(){
        return sdf_return(-dist, color);
    }

    sdf_return operator-(float o){
        return sdf_return(dist - o, color);
    }
};

class sdf{
public:
    float sharpness = 0.3f;
    float throatLength = 1;
    float wormholeRad = 0.5f;
    vec4 wormholePos = vec4(0,1,0,throatLength/2.0f);

    sdf(){}
    /*
    sdf_return sdf_4d_slice(vec4 p, float w){
        return std::abs(p.w - w);
    }
    */
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

    sdf_return sdf_plane_2d(vec4 p, vec4 c, vec4 n1, vec4 n2){
        return sdf_return(sqrt((p-c).dot(n1) * (p-c).dot(n1) + (p-c).dot(n2) * (p-c).dot(n2)));
    }

    sdf_return union_round(sdf_return a, sdf_return b, float r){
        vec2 q = vec2(r - a.dist, r - b.dist).max(0.0f);
        sdf_return closer = a.min(b);
        float final_dist = std::max(closer.dist, r) - q.mag();
        return sdf_return(final_dist, closer.color);
    }

    sdf_return space_sdf(vec4 p){
        /*
        sdf_return slice0 = sdf_4d_slice(p, 0);
        slice0.color = 0xFFFF0000;
        sdf_return slice1 = sdf_4d_slice(p, throatLength);
        slice1.color = 0xFF00FF00;
        sdf_return slices = slice0.min(slice1);

        sdf_return wormhole = sdf_extrude_sphere(p, vec4(0,1,0,throatLength/2.0f), 0.2f, throatLength);
        wormhole.color = 0xFF0000FF;

        sdf_return slicescut = slices.max(-wormhole);

        return union_round(slicescut, wormhole, sharpness);
        */

        /*
        sdf_return space0 = std::abs(p.w - throatLength/2) - throatLength/2;
        sdf_return wormhole0 = sdf_extrude_sphere(p, wormholePos, wormholeRad, throatLength);
        //float subtract = std::min(slice, wormhole0);

        sdf_return a = space0;
        sdf_return b = -wormhole0;

        return union_round(a,b,sharpness);
        */

        sdf_return space0 = sdf_sphere_4d(p, vec4(0,0,0,-10), 10);
        space0.color = 0xFFFF0000;
        sdf_return space1 = sdf_sphere_4d(p, vec4(0,0,0,10 + throatLength), 10);
        space1.color = 0xFF00FF00;
        sdf_return wormhole0 = sdf_extrude_sphere(p, wormholePos, wormholeRad, throatLength);
        //float subtract = std::min(slice, wormhole0);

        sdf_return a = space0.min(space1);
        sdf_return b = wormhole0;

        return union_round(a,b,sharpness);
    }

    sdf_return scene_sdf(vec4 p){
        sdf_return floor = p.y;
        floor.color = space_sdf(p).color;

        return floor;

    }

    vec4 spaceNorm(vec4 p){
        float d = space_sdf(p).dist;
        vec2 t = vec2(0.001f, 0.0f);

        vec4 v1 = (vec4(space_sdf(p-vec4(t.x, t.y, t.y, t.y)).dist, space_sdf(p-vec4(t.y, t.x, t.y, t.y)).dist, space_sdf(p-vec4(t.y, t.y, t.x, t.y)).dist, space_sdf(p-vec4(t.y, t.y, t.y, t.x)).dist)).sub_before(d);
        vec4 v2 = (vec4(space_sdf(p+vec4(t.x, t.y, t.y, t.y)).dist, space_sdf(p+vec4(t.y, t.x, t.y, t.y)).dist, space_sdf(p+vec4(t.y, t.y, t.x, t.y)).dist, space_sdf(p+vec4(t.y, t.y, t.y, t.x)).dist)).sub_before(d);
        //return (v1-v2).normalize();
        vec4 g = v1 - v2;
        float m = g.mag();
        if (m < 1e-8f) return vec4(0,0,0,0);
        return g / m;

    }

    sdf_return raymarch(vec4 p, vec4 dir){
        float dist = scene_sdf(p).dist;
        sdf_return moved = sdf_return(0);
        int minCounter = 0;

        while(dist > 0.001){
            if(moved.dist >= 100){
                return -1;
            }

            sdf_return throat = sdf_extrude_sphere(p, wormholePos, wormholeRad, throatLength);

            float minStep = std::min(0.005f, 0.5f * sharpness);
            float step = std::min(dist, std::max(throat.dist - sharpness, minStep));

            p = p + dir * step;
            moved.dist += step;
            if(step == 0.005f){
                minCounter += 1;
                if(minCounter >= 2000){
                    return -1;
                }
            } else{
                minCounter = 0;
            }

            for (int i = 0; i < 3; ++i) {
                float d = space_sdf(p).dist;
                if (std::abs(d) < 1e-5f) break;
                vec4 n = spaceNorm(p);
                if (n.mag() == 0.0f) break;
                p = p - n * d;
            }


            vec4 n = spaceNorm(p);
            if (n.mag() > 0.0f) {
                vec4 t = dir - n * dir.dot(n);
                if (t.mag() > 1e-6f) dir = t.normalize();
            }

            /*
            sdf_return throat = sdf_extrude_sphere(p, vec4(0,1,0,throatLength/2.0f), 0.2f, throatLength);

            float step = (dist <= throat.dist) ? dist : throat.dist;
            if(step < 0.005f){
                step = 0.005f;
            }

            vec4 prev_p = p;
            float prev_dist = space_sdf(p).dist;

            p = p + dir * step;
            moved.dist += step;

            float curr_dist = space_sdf(p).dist;

            if(std::abs(curr_dist) != 0.0f){
                vec4 norm = spaceNorm(p);
                p = p - norm * curr_dist;
                dir = (dir - norm * dir.dot(norm)).normalize();
            }
            */

            dist = scene_sdf(p).dist;
        }
        moved.color = scene_sdf(p).color;
        return moved;
    }

    sdf_return raymarch_space(vec4 p, vec4 dir){
        float dist = space_sdf(p).dist;
        sdf_return moved = sdf_return(0);

        while(dist > 0.01){
            if(moved.dist >= 10){
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
