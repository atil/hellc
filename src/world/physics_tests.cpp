#include <iostream>
#include "geom.h"
#include "world.h"

void Physics::run_collision_tests() {
    std::cout << "Running collision tests " << std::endl;

    // Triangle below player
    PlayerShape ps(Vector3(0, 3, 0), 1, 0.5f);
    Triangle t1(Vector3(-1, 0, 1), Vector3(1, 0, 1), Vector3(0, 0, -1));
    Vector3 penet;
    bool has_penet = resolve_penetration(ps, t1, penet);
    assert(!has_penet);

    // Triangle intersecting player -- outside the segment
    ps = PlayerShape(Vector3(0, 0.75f, 0), 1, 0.5f);
    t1 = Triangle(Vector3(-1, 0, 1), Vector3(1, 0, 1), Vector3(0, 0, -1));
    has_penet = resolve_penetration(ps, t1, penet);
    assert(has_penet);
    assert(penet == Vector3(0, 0.25f, 0));

    // Triangle intersecting player -- within the segment
    ps = PlayerShape(Vector3(0, 0, 0), 1, 0.5f);
    t1 = Triangle(Vector3(-1, -0.25f, 1), Vector3(1, -0.25f, 1), Vector3(0, -0.25f, -1));
    has_penet = resolve_penetration(ps, t1, penet);
    assert(has_penet);
    assert(penet == Vector3(0, 0.75f, 0));

    // Triangle is _just_ below the player
    ps = PlayerShape(Vector3(0, 1, 0), 1, 0.5f);
    t1 = Triangle(Vector3(-1, 0, 1), Vector3(1, 0, 1), Vector3(0, 0, -1));
    has_penet = resolve_penetration(ps, t1, penet);
    assert(has_penet);
    assert(penet == Vector3(0, 0, 0));

    // Vertical triangle -- not intersecting
    ps = PlayerShape(Vector3(0, 0, 0), 1, 0.5f);
    t1 = Triangle(Vector3(10, -1, -1), Vector3(10, -1, 1), Vector3(10, 1, 0));
    has_penet = resolve_penetration(ps, t1, penet);
    assert(!has_penet);

    // Vertical triangle -- right on the edge
    ps = PlayerShape(Vector3(0, 0, 0), 1, 0.5f);
    t1 = Triangle(Vector3(0.5f, -1, -1), Vector3(0.5f, -1, 1), Vector3(0.5f, 1, 0));
    has_penet = resolve_penetration(ps, t1, penet);
    assert(has_penet);
    assert(penet == Vector3(0, 0, 0));

    // Problem #1
    ps = PlayerShape(Vector3(0, 3, 1), 1, 0.5f);
    t1 = Triangle(Vector3(0, 0, 0), Vector3(0, 0, 10), Vector3(0, 5, 10));
    has_penet = resolve_penetration(ps, t1, penet);
    assert(!has_penet);

    // Problem #2
    ps = PlayerShape(Vector3(-0.45f, 3, 1), 1, 0.5f);
    t1 = Triangle(Vector3(0, 5, 0), Vector3(0, 0, 0), Vector3(0, 5, 10));
    has_penet = resolve_penetration(ps, t1, penet);
    assert(has_penet);
    assert(penet == Vector3(-0.0500000119f, 0, 0));


    // Problem #3
    ps = PlayerShape(Vector3(-3.01f, 0.93f, -3.12f), 1, 0.5f);
    t1 = Triangle(Vector3(-10, 0, 0), Vector3(0, 0, 0), Vector3(-10, 0, -10));
    has_penet = resolve_penetration(ps, t1, penet);
    assert(has_penet);
    assert(approx_vec(penet, Vector3(0, 0.07f, 0)));

    std::cout << "Success" << std::endl;
}