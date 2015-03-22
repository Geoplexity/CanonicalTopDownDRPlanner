#ifndef VISION_HANDLER_2D_HPP
#define VISION_HANDLER_2D_HPP


#include "../../ext/glm/glm.hpp"
#include "../../ext/glm/gtc/matrix_transform.hpp"
#include "../../ext/glm/gtx/rotate_vector.hpp"
// #include "../ext/glm/gtc/matrix_transform.hpp"
// #include "../ext/glm/gtx/vector_angle.hpp"


// TODO: Deal with aspect ratio....



namespace VisionHandlerNS {
    enum directions {
        LEFT,
        RIGHT,
        UP,
        DOWN
    };
}


class Vision_Handler_2D {
public:
    // Vision_Handler_2D(glm::vec2 center, glm::vec2 up, float aspectRatio);
    Vision_Handler_2D(float aspect_ratio);

    // BEGIN: Getters and Setters
    /////////////////////////////
    void aspect_ratio(float r);
    float aspect_ratio();

    void center(glm::vec2 c);
    glm::vec2 center();

    void up(glm::vec2 u);
    glm::vec2 up();

    void zoom(float z);
    float zoom();
    /////////////////////////////
    // END:   Getters and Setters



    void revert_to_original();

    glm::mat4 get_view_matrix();
    glm::mat4 get_projection_matrix();



    // BEGIN: Eye Modifications
    ///////////////////////////
    void turn_right(float degrees);

    void translate(float x, float y);

    void scale_zoom(float ratio, float min, float max);


    // void moveOnXZPlaneForwardBackward(float distance);

    // void moveOnXZPlaneRightLeft(float distance);


    ///////////////////////////
    // END:   Eye Modifications


    // BEGIN: Center Modifications
    //////////////////////////////
    // void lookUpDown(float degrees);

    // void lookRightLeft(float degrees);
    //////////////////////////////
    // END:   Center Modifications
private:
    // for the view
    glm::vec2 _center_default, _center;
    glm::vec2 _up_default,     _up;
    float     _zoom_default,   _zoom;

    // for the projection
    float _aspect_ratio;

    // // ortho
    // static const float ortho_bottom_default = -5.f, ortho_left_default  = -5.f;
    // static const float ortho_top_default    =  5.f, ortho_right_default =  5.f;
    // // perspective
    // static const float perspective_fovy_default = 50.f;
};



#endif
