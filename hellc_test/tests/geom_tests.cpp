#include "tests.h"

void test_line_segment_above_triangle() {
    Vector3 closer_point;
    const Vector3 a(0, 1, 0);
    const Vector3 b(0, 2, 0);
    const Triangle t1(Vector3(-1, 0, 1), Vector3(1, 0, 1), Vector3(0, 0, -1));
    const float d1 = get_line_segment_plane_distance(a, b, t1, closer_point);
    assert(approx(d1, 1.0f));
    assert(closer_point == a);
}

void test_line_segment_crosses_triangle() {
    Vector3 intersection_point;
    const Vector3 a(0, 1, 0);
    const Vector3 b(0, -2, 0);
    const Triangle t1(Vector3(-1, 0, 1), Vector3(1, 0, 1), Vector3(0, 0, -1));
    const float d1 = get_line_segment_plane_distance(a, b, t1, intersection_point);
    assert(approx(d1, 0.0f));
}

void test_line_segment_parallel_to_triangle() {
    Vector3 closer_point;
    const Vector3 a(1, 1, 0);
    const Vector3 b(0, 1, 0);
    const Triangle t1(Vector3(-1, 0, 1), Vector3(1, 0, 1), Vector3(0, 0, -1));
    const float d1 = get_line_segment_plane_distance(a, b, t1, closer_point);
    assert(approx(d1, 1.0f));
    assert(closer_point == (a + b) * 0.5f);
}

void test_project_point_to_triangle_plane() {
    const Vector3 a(1, 0, 0);
    const Triangle t1(Vector3(-1, 0, 1), Vector3(1, 0, 1), Vector3(0, 0, -1));
    float dist;
    const Vector3 projected_point = project_point_on_triangle_plane(a, t1, dist);
    assert(projected_point == Vector3(1, 0, 0));
    assert(approx(dist, 0.0f));
}

void test_point_in_triangle_inside() {
    const Vector3 a(0, 0, 0);
    const Triangle t1(Vector3(-1, 0, 1), Vector3(1, 0, 1), Vector3(0, 0, -1));
    const bool b = is_point_in_triangle(a, t1);
    assert(b);
}

void test_point_in_triangle_outside() {
    const Vector3 a(10, 0, 0);
    const Triangle t1(Vector3(-1, 0, 1), Vector3(1, 0, 1), Vector3(0, 0, -1));
    const bool b = is_point_in_triangle(a, t1);
    assert(!b);
}

void test_point_in_triangle_on_counts_as_inside() {
    const Vector3 a(0, 0, -1);
    const Triangle t1(Vector3(-1, 0, 1), Vector3(1, 0, 1), Vector3(0, 0, -1));
    const bool b = is_point_in_triangle(a, t1);
    assert(b);
}

void test_closest_point_on_triangle_outside() {
    const Vector3 a(10, 0, 0);
    const Triangle t1(Vector3(-1, 0, 1), Vector3(1, 0, 1), Vector3(0, 0, -1));
    const Vector3 closest_point = get_closest_point_on_triangle(a, t1);
    assert(closest_point == Vector3(1, 0, 1));
}

void test_closest_point_on_triangle_on() {
    const Vector3 a(-1, 0, 1);
    const Triangle t1(Vector3(-1, 0, 1), Vector3(1, 0, 1), Vector3(0, 0, -1));
    const Vector3 closest_point = get_closest_point_on_triangle(a, t1);
    assert(closest_point == Vector3(-1, 0, 1));
}

void test_problem_1() {
    const Vector3 a(0, 2, 1);
    const Triangle t1 = Triangle(Vector3(0, 0, 0), Vector3(0, 0, 10), Vector3(0, 5, 10));
    const bool b = is_point_in_triangle(a, t1);
    assert(!b);
}

void run_geom_tests() {
    std::cout << "Running geom tests " << std::endl;

    test_line_segment_above_triangle();
    test_line_segment_crosses_triangle();
    test_line_segment_parallel_to_triangle();
    test_project_point_to_triangle_plane();
    test_point_in_triangle_inside();
    test_point_in_triangle_outside();
    test_point_in_triangle_on_counts_as_inside();
    test_closest_point_on_triangle_outside();
    test_closest_point_on_triangle_on();
    test_problem_1();

    std::cout << "Success" << std::endl;
}
