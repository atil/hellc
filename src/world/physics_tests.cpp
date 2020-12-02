#include <iostream>
#include "geom.h"
#include "world.h"

void Physics::run_collision_tests() {
    std::cout << "Running collision tests " << std::endl;

    // Triangle below player
    PlayerShape ps(glm::vec3(0, 3, 0), 1, 0.5);
    Triangle t1(glm::vec3(-1, 0, 1), glm::vec3(1, 0, 1), glm::vec3(0, 0, -1));
    glm::vec3 penet;
    bool has_penet = resolve_penetration(ps, t1, penet);
    assert(!has_penet);

    // Triangle intersecting player -- outside the segment
    ps = PlayerShape(glm::vec3(0, 0.75f, 0), 1, 0.5);
    t1 = Triangle(glm::vec3(-1, 0, 1), glm::vec3(1, 0, 1), glm::vec3(0, 0, -1));
    has_penet = resolve_penetration(ps, t1, penet);
    assert(has_penet);
    assert(penet == glm::vec3(0, 0.25f, 0));

    // Triangle intersecting player -- within the segment
    ps = PlayerShape(glm::vec3(0, 0, 0), 1, 0.5);
    t1 = Triangle(glm::vec3(-1, -0.25, 1), glm::vec3(1, -0.25, 1), glm::vec3(0, -0.25, -1));
    has_penet = resolve_penetration(ps, t1, penet);
    assert(has_penet);
    assert(penet == glm::vec3(0, 0.75f, 0));

    // Triangle is _just_ below the player
    ps = PlayerShape(glm::vec3(0, 1, 0), 1, 0.5);
    t1 = Triangle(glm::vec3(-1, 0, 1), glm::vec3(1, 0, 1), glm::vec3(0, 0, -1));
    has_penet = resolve_penetration(ps, t1, penet);
    assert(has_penet);
    assert(penet == glm::vec3(0, 0, 0));

    // Vertical triangle -- not intersecting
    ps = PlayerShape(glm::vec3(0, 0, 0), 1, 0.5);
    t1 = Triangle(glm::vec3(10, -1, -1), glm::vec3(10, -1, 1), glm::vec3(10, 1, 0));
    has_penet = resolve_penetration(ps, t1, penet);
    assert(!has_penet);

    // Vertical triangle -- right on the edge
    ps = PlayerShape(glm::vec3(0, 0, 0), 1, 0.5);
    t1 = Triangle(glm::vec3(0.5, -1, -1), glm::vec3(0.5, -1, 1), glm::vec3(0.5, 1, 0));
    has_penet = resolve_penetration(ps, t1, penet);
    assert(has_penet);
    assert(penet == glm::vec3(0, 0, 0));

    // Problem #1
    ps = PlayerShape(glm::vec3(0, 3, 1), 1, 0.5);
    t1 = Triangle(glm::vec3(0, 0, 0), glm::vec3(0, 0, 10), glm::vec3(0, 5, 10));
    has_penet = resolve_penetration(ps, t1, penet);
    assert(!has_penet);

    // Problem #2
    ps = PlayerShape(glm::vec3(-0.45, 3, 1), 1, 0.5);
    t1 = Triangle(glm::vec3(0, 5, 0), glm::vec3(0, 0, 0), glm::vec3(0, 5, 10));
    has_penet = resolve_penetration(ps, t1, penet);
    assert(has_penet);
    assert(penet == glm::vec3(-0.0500000119f, 0, 0));


    // Problem #3
    ps = PlayerShape(glm::vec3(-3.01, 0.93, -3.12), 1, 0.5);
    t1 = Triangle(glm::vec3(-10, 0, 0), glm::vec3(0, 0, 0), glm::vec3(-10, 0, -10));
    has_penet = resolve_penetration(ps, t1, penet);
    assert(has_penet);
    assert(approx_vec(penet, glm::vec3(0, 0.07, 0)));

    std::cout << "Success" << std::endl;
}