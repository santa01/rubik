/*
 * Copyright (c) 2013 Pavlo Lavrenenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef CAMERA_H
#define CAMERA_H

#include "Mat4.h"
#include "Movable.h"
#include "Rotatable.h"

namespace Rubik {

namespace Game {

class Camera: public Common::Movable, public Common::Rotatable {
public:
    enum ProjectionType {
        TYPE_PERSPECTIVE,
        TYPE_ORTHOGRAPHIC
    };

    Camera() {
        this->aspectRatio = 1.3333f;
        this->nearPlane = 0.1f;
        this->farPlane = 20.0f;
        this->fov = 90.0f;
        this->projectionType = TYPE_PERSPECTIVE;

        this->updateProjection();
        this->lookAt(Math::Vec3::UNIT_Z);
    }

    Camera(float x, float y, float z):
            Camera() {
        this->setPosition(x, y, z);
    }

    Camera(const Math::Vec3& position):
            Camera() {
        this->setPosition(position);
    }

    using Common::Movable::setPosition;

    void setPosition(const Math::Vec3& position) {
        this->translation.set(0, 3, -position.get(Math::Vec3::X));
        this->translation.set(1, 3, -position.get(Math::Vec3::Y));
        this->translation.set(2, 3, -position.get(Math::Vec3::Z));
    }

    Math::Vec3 getPosition() const {
        return Math::Vec3(-this->translation.get(0, 3),
                          -this->translation.get(1, 3),
                          -this->translation.get(2, 3));
    }

    float getXAngle() const {
        return this->xAngle;
    }

    float getYAngle() const {
        return this->yAngle;
    }

    float getZAngle() const {
        return this->zAngle;
    }

    void rotate(const Math::Vec3& vector, float angle);

    const Math::Mat4& getTranslation() const {
        return this->translation;
    }

    const Math::Mat4& getRotation() const {
        return this->rotation;
    }

    const Math::Mat4& getProjection() const {
        return this->projection;
    }

    ProjectionType getProjectionType() const {
        return this->projectionType;
    }

    void setProjectionType(ProjectionType type) {
        this->projectionType = type;
        this->updateProjection();
    }

    float getAspectRatio() const {
        return this->aspectRatio;
    }

    void setAspectRatio(float aspectRatio) {
        this->aspectRatio = aspectRatio;
        this->updateProjection();
    }

    float getNearPlane() const {
        return this->nearPlane;
    }

    void setNearPlane(float nearPlane) {
        this->nearPlane = nearPlane;
        this->updateProjection();
    }

    float getFarPlane() const {
        return this->farPlane;
    }

    void setFarPlane(float farPlane) {
        this->farPlane = farPlane;
        this->updateProjection();
    }

    float getFov() const {
        return this->fov;
    }

    void setFov(float fov) {
        this->fov = fov;
        this->updateProjection();
    }

    Math::Vec3 getUp() const {
        return Math::Vec3(this->rotation.get(1, 0),
                          this->rotation.get(1, 1),
                          this->rotation.get(1, 2));
    }

    Math::Vec3 getTarget() const {
        return -Math::Vec3(this->rotation.get(2, 0),
                           this->rotation.get(2, 1),
                           this->rotation.get(2, 2));
    }

    Math::Vec3 getRight() const {
        Math::Vec3 up(this->rotation.get(1, 0),
                      this->rotation.get(1, 1),
                      this->rotation.get(1, 2));
        Math::Vec3 target(this->rotation.get(2, 0),
                          this->rotation.get(2, 1),
                          this->rotation.get(2, 2));
        Math::Vec3 right = target.cross(up);
        return right.normalize();
    }

    void lookAt(float x, float y, float z) {
        this->lookAt(Math::Vec3(x, y, z));
    }

    void lookAt(const Math::Vec3& target);

private:
    void updateProjection();
    void updateRotation(const Math::Vec3& right, const Math::Vec3& up, const Math::Vec3& target);

    ProjectionType projectionType;

    Math::Mat4 projection;
    Math::Mat4 translation;
    Math::Mat4 rotation;

    float aspectRatio;
    float nearPlane;
    float farPlane;
    float fov;

    float xAngle;
    float yAngle;
    float zAngle;
};

}  // namespace Game

}  // namespace Rubik

#endif  // CAMERA_H
