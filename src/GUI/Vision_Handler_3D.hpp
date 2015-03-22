#ifndef VISIONHANDLER_HPP
#define VISIONHANDLER_HPP


#include "../ext/glm/glm.hpp"
// #include "../ext/glm/gtc/matrix_transform.hpp"
// #include "../ext/glm/gtx/vector_angle.hpp"


namespace VisionHandlerNS {
    enum projType {
        ORTHOGRAPHIC,
        PERSPECTIVE
    };

    enum directions {
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    // ortho
    const float ortho_bottom_default = -5.f, ortho_left_default  = -5.f;
    const float ortho_top_default    =  5.f, ortho_right_default =  5.f;
    // perspective
    const float perspective_fovy_default = 50.f;
}


class VisionHandler {
public:
    VisionHandler(glm::vec3 eye, glm::vec3 center, glm::vec3 up, VisionHandlerNS::projType proj, float aspectRatio) {
        _eye_default    = eye;    _eye    = eye;
        _center_default = center; _center = center;
        _up_default     = up;     _up     = up;

        _proj_default   = proj;   _proj   = proj;
        _aspect_ratio   = aspectRatio;
    }

    // BEGIN: Getters and Setters
    /////////////////////////////
    void eye(glm::vec3 e) {_eye = e;}
    glm::vec3 eye() {return _eye;}

    void center(glm::vec3 c) {_center = c;}
    glm::vec3 center() {return _center;}

    void up(glm::vec3 u) {_up = u;}
    glm::vec3 up() {return _up;}

    void ecu(glm::vec3 e, glm::vec3 c, glm::vec3 u) {
        eye(e); center(c); up(u);
    }

    void proj(VisionHandlerNS::projType p) {_proj = p;}
    VisionHandlerNS::projType proj() {return _proj;}
    /////////////////////////////
    // END:   Getters and Setters



    void revertToOriginal() {
        eye(_eye_default);
        center(_center_default);
        up(_up_default);

        proj(_proj_default);
    }

    glm::mat4 getViewMatrix() {
        return glm::lookAt(_eye, _center, _up);
    }

    // glm::mat4 getProjectionMatrix() {
    //     if (proj() == VisionHandlerNS::ORTHOGRAPHIC) {
    //         return glm::ortho(
    //             ortho_left_default,
    //             ortho_right_default,
    //             ortho_bottom_default,
    //             ortho_top_default,
    //             0.1f, 100.f
    //         );
    //     } else if (proj() == VisionHandlerNS::PERSPECTIVE) {
    //         return glm::perspective(
    //             perspective_fovy_default,
    //             _aspect_ratio,
    //             0.1f, 100.f
    //         );
    //     }
    // }

    glm::mat4 getProjectionMatrix() {
        if (proj() == VisionHandlerNS::ORTHOGRAPHIC) {
            return glm::ortho(
                VisionHandlerNS::ortho_left_default,
                VisionHandlerNS::ortho_right_default,
                VisionHandlerNS::ortho_bottom_default,
                VisionHandlerNS::ortho_top_default,
                0.1f, 100.f
            );
        } else {// if (proj() == VisionHandlerNS::PERSPECTIVE) {

            // default to persepctive
            return glm::perspective(
                VisionHandlerNS::perspective_fovy_default,
                _aspect_ratio,
                0.1f, 100.f
            );
        }
    }



    // BEGIN: Eye Modifications
    ///////////////////////////
    void rotateEyeAboutVector(glm::vec3 v, float degrees) {
        glm::vec4 eye2 = glm::vec4(eye(), 0);
        eye2 = glm::rotate(glm::mat4(1.f), degrees, v) * eye2;
        eye(glm::vec3(eye2));
    }

    void rotateEyeAboutYAxis(float degrees) {
        rotateEyeAboutVector(glm::vec3(0.f, 1.f, 0.f), degrees);
    }

    void rotateEyeTowardsPole(float degrees) {
        rotateEyeAboutVector(
            glm::cross(eye(), glm::vec3(0.f, 1.f, 0.f)),
            degrees
        );
    }

    void zoomIn(float ratio, float cap) {
        glm::vec3 newCenter2Eye = (eye()-center())*ratio;
        if (cap < glm::length(newCenter2Eye)) eye(newCenter2Eye+center());
    }

    void zoomOut(float ratio, float cap) {
        glm::vec3 newCenter2Eye = (eye()-center())*ratio;
        if (cap > glm::length(newCenter2Eye)) eye(newCenter2Eye+center());
    }

    void translate(glm::vec3 t) {
        eye(eye()+t);
        center(center()+t);
    }

    void moveOnXZPlaneForwardBackward(float distance) {
        glm::vec3 t = center()-eye();
        t[1] = 0.f;
        //protects from moving while looking directly up
        if (glm::length(t) > 0.00001) {
            t = glm::normalize(t);
            t*=distance;
            translate(t);
        }
    }

    void moveOnXZPlaneRightLeft(float distance) {
        glm::vec3 t = center()-eye();
        t[1] = t[0];
        t[0] = -t[2];
        t[2] = t[1];
        t[1] = 0.f;
        //protects from moving while looking directly up
        if (glm::length(t) > 0.00001) {
            t = glm::normalize(t);
            t*=distance;
            translate(t);
        }
    }


    ///////////////////////////
    // END:   Eye Modifications


    // BEGIN: Center Modifications
    //////////////////////////////
    void lookUpDown(float degrees) {
        glm::vec4 eye2center = glm::vec4(center()-eye(), 1.f);
        eye2center = glm::rotate( glm::mat4(1.f), degrees, glm::cross(glm::vec3(eye2center), up()) )
            * eye2center;
        center(glm::vec3(eye2center) + eye());
    }

    void lookRightLeft(float degrees) {
        glm::vec4 eye2center = glm::vec4(center()-eye(), 1.f);
        eye2center = glm::rotate( glm::mat4(1.f), -degrees, up() )
            * eye2center;
        center(glm::vec3(eye2center) + eye());
    }
    //////////////////////////////
    // END:   Center Modifications
private:
    // for the view
    glm::vec3 _eye_default,    _eye;
    glm::vec3 _center_default, _center;
    glm::vec3 _up_default,     _up;

    // for the projection
    VisionHandlerNS::projType _proj_default, _proj;
    float _aspect_ratio;

    // // ortho
    // static const float ortho_bottom_default = -5.f, ortho_left_default  = -5.f;
    // static const float ortho_top_default    =  5.f, ortho_right_default =  5.f;
    // // perspective
    // static const float perspective_fovy_default = 50.f;
};



#endif
