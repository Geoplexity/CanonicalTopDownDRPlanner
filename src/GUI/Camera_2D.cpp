#include "Camera_2D.hpp"



// TODO: Deal with aspect ratio....



// Camera_2D::Camera_2D(glm::vec2 center, glm::vec2 up, float aspectRatio) {
//     _center_default = center; _center = _center_default;
//     _up_default     = up;     _up     = _up_default;
//     _zoom_default   = 1.f;    _zoom   = _zoom_default;

//     _aspect_ratio   = aspectRatio;
// }

Camera_2D::Camera_2D(float aspect_ratio) {
    _center_default = glm::vec2(0.f, 0.f); _center = _center_default;
    _up_default     = glm::vec2(0.f, 1.f); _up     = _up_default;
    _zoom_default   = 1.f;                 _zoom   = _zoom_default;

    _aspect_ratio   = aspect_ratio;
}

// BEGIN: Getters and Setters
/////////////////////////////
void Camera_2D::aspect_ratio(const float r) {_aspect_ratio = r;}
float Camera_2D::aspect_ratio() const {return _aspect_ratio;}

void Camera_2D::center(const glm::vec2 c) {_center = c;}
glm::vec2 Camera_2D::center() const {return _center;}

void Camera_2D::up(const glm::vec2 u) {_up = u;}
glm::vec2 Camera_2D::up() const {return _up;}

void Camera_2D::zoom(const float z) {_zoom = z;}
float Camera_2D::zoom() const {return _zoom;}
/////////////////////////////
// END:   Getters and Setters



void Camera_2D::revert_to_original() {
    center(_center_default);
    up(_up_default);
    zoom(_zoom_default);
}

glm::mat4 Camera_2D::get_view_matrix() {
    return glm::scale(
        glm::lookAt(
            glm::vec3(center(), 1.f), // the "eye" vector
            glm::vec3(center(), 0.f),
            glm::vec3(up(), 0.f)),
        glm::vec3(zoom(), zoom(), zoom()));
}

// glm::mat4 Camera_2D::get_projection_matrix() {
//     float right, top;
//     if (this->_aspect_ratio > 1) {
//         top   = 1;
//         right = _aspect_ratio;
//     } else {
//         top   = _aspect_ratio;
//         right = 1;
//     }
//     return glm::ortho(-right, right, -top, top);
// }

glm::mat4 Camera_2D::get_projection_matrix() {
    // create identity matrix
    glm::mat4 pmat;
    for (int i = 0; i<4; i++)
        for (int j = 1; j<4; j++)
            pmat[i][j] = (i==j)? 1: 0;

    // scale x or y axis depending on ratio
    if (this->_aspect_ratio > 1)
        pmat[0][0] = float(1)/_aspect_ratio;
    else
        pmat[1][1] = _aspect_ratio;

    return pmat;
}

glm::mat4 Camera_2D::get_projection_view_matrix() {
    return get_projection_matrix() * get_view_matrix();
}



// glm::mat4 Camera_2D::get_view_matrix() {
//     // glm::mat4 persp glm::perspective(
//     //         VisionHandlerNS::perspective_fovy_default,
//     //         _aspect_ratio,
//     //         0.1f, 100.f
//     //     );
//     return glm::lookAt(
//         glm::vec3(center(), zoom()), // the "eye" vector
//         glm::vec3(center(), 0.f),
//         glm::vec3(up(), 0.f));
// }


// BEGIN: Eye Modifications
///////////////////////////
void Camera_2D::turn_right(float degrees) {
    up(glm::rotate(up(), degrees));
}

void Camera_2D::translate(float x, float y) {
    center(center() + glm::vec2(x, y));
}

void Camera_2D::scale_zoom(float ratio, float min, float max) {
    float new_zoom = zoom()*ratio;
    if (min < new_zoom && max > new_zoom) zoom(new_zoom);
}


// void Camera_2D::moveOnXZPlaneForwardBackward(float distance) {
//     glm::vec2 t = center()-eye();
//     t[1] = 0.f;
//     //protects from moving while looking directly up
//     if (glm::length(t) > 0.00001) {
//         t = glm::normalize(t);
//         t*=distance;
//         translate(t);
//     }
// }

// void Camera_2D::moveOnXZPlaneRightLeft(float distance) {
//     glm::vec2 t = center()-eye();
//     t[1] = t[0];
//     t[0] = -t[2];
//     t[2] = t[1];
//     t[1] = 0.f;
//     //protects from moving while looking directly up
//     if (glm::length(t) > 0.00001) {
//         t = glm::normalize(t);
//         t*=distance;
//         translate(t);
//     }
// }


///////////////////////////
// END:   Eye Modifications


// BEGIN: Center Modifications
//////////////////////////////
// void Camera_2D::lookUpDown(float degrees) {
//     glm::vec4 eye2center = glm::vec4(center()-eye(), 1.f);
//     eye2center = glm::rotate( glm::mat4(1.f), degrees, glm::cross(glm::vec2(eye2center), up()) )
//         * eye2center;
//     center(glm::vec2(eye2center) + eye());
// }

// void Camera_2D::lookRightLeft(float degrees) {
//     glm::vec4 eye2center = glm::vec4(center()-eye(), 1.f);
//     eye2center = glm::rotate( glm::mat4(1.f), -degrees, up() )
//         * eye2center;
//     center(glm::vec2(eye2center) + eye());
// }
//////////////////////////////
// END:   Center Modifications

