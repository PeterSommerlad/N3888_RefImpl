#include "drawing.h"
#include "xdrawinghelpers.h"
#include "xcairoenumhelpers.h"

using namespace std;
using namespace std::experimental::drawing;

surface::context::native_handle_type surface::context::native_handle() const {
    return _Context.get();
}

surface::context::context(surface::context&& other)
: _Context(nullptr) {
    _Context = move(other._Context);
    other._Context = nullptr;
}

surface::context& surface::context::operator=(surface::context&& other) {
    if (this != &other) {
        _Context = move(other._Context);
    }
    other._Context = nullptr;
    return *this;
}

surface::context::context(const surface& s)
: _Context(nullptr) {
    _Context = shared_ptr<cairo_t>(cairo_create(s.native_handle()), &cairo_destroy);
}

//surface::context::context(surface::context::native_handle_type nh)
//: _Surface(surface(cairo_surface_reference(cairo_get_target(nh)))) {
//    _Context = shared_ptr<cairo_t>(nh, &cairo_destroy);
//}

status surface::context::status() {
    return _Cairo_status_t_to_status(cairo_status(_Context.get()));
}

void surface::context::save() {
    cairo_save(_Context.get());
}

void surface::context::restore() {
    cairo_restore(_Context.get());
}

// We're making context always obtained from its surface such that this functopm no longer makes sense.
//surface& surface::context::get_target() {
//    return *_Surface;
//}

void surface::context::push_group() {
    cairo_push_group(_Context.get());
}

void surface::context::push_group_with_content(content c) {
    cairo_push_group_with_content(_Context.get(), _Content_to_cairo_content_t(c));
}

pattern surface::context::pop_group() {
    return pattern(cairo_pop_group(_Context.get()));
}

void surface::context::pop_group_to_source() {
    cairo_pop_group_to_source(_Context.get());
}

// Eliminate this function since it can sometimes return the context's surface.
//surface surface::context::get_group_target() {
//    // Since we have no idea which surface we're getting back we must use cairo's referencing to ensure proper lifetime.
//    return surface(cairo_surface_reference(cairo_get_group_target(_Context.get())));
//}

void surface::context::set_source_rgb(double red, double green, double blue) {
    cairo_set_source_rgb(_Context.get(), red, green, blue);
}

void surface::context::set_source_rgba(double red, double green, double blue, double alpha) {
    cairo_set_source_rgba(_Context.get(), red, green, blue, alpha);
}

void surface::context::set_source(const pattern& source) {
    // This is not dangerous because cairo will reference (and destroy) whenever its pattern changes.
    cairo_set_source(_Context.get(), source.native_handle());
}

void surface::context::set_source_surface(const surface& s, double x, double y) {
    cairo_set_source_surface(_Context.get(), s.native_handle(), x, y);
}

void surface::context::set_antialias(antialias a) {
    cairo_set_antialias(_Context.get(), _Antialias_to_cairo_antialias_t(a));
}

antialias surface::context::get_antialias() {
    return _Cairo_antialias_t_to_antialias(cairo_get_antialias(_Context.get()));
}

void surface::context::set_dash(const vector<double>& dashes, double offset) {
    cairo_set_dash(_Context.get(), dashes.data(), static_cast<int>(dashes.size()), offset);
}

int surface::context::get_dash_count() {
    return cairo_get_dash_count(_Context.get());
}

void surface::context::get_dash(vector<double>& dashes, double& offset) {
    dashes.resize(get_dash_count());
    cairo_get_dash(_Context.get(), &dashes[0], &offset);
}

void surface::context::set_fill_rule(fill_rule fr) {
    cairo_set_fill_rule(_Context.get(), _Fill_rule_to_cairo_fill_rule_t(fr));
}

fill_rule surface::context::get_fill_rule() {
    return _Cairo_fill_rule_t_to_fill_rule(cairo_get_fill_rule(_Context.get()));
}

void surface::context::set_line_cap(line_cap lc) {
    cairo_set_line_cap(_Context.get(), _Line_cap_to_cairo_line_cap_t(lc));
}

line_cap surface::context::get_line_cap() {
    return _Cairo_line_cap_t_to_line_cap(cairo_get_line_cap(_Context.get()));
}

void surface::context::set_line_join(line_join lj) {
    cairo_set_line_join(_Context.get(), _Line_join_to_cairo_line_join_t(lj));
}

line_join surface::context::get_line_join() {
    return _Cairo_line_join_t_to_line_join(cairo_get_line_join(_Context.get()));
}

void surface::context::set_line_width(double width) {
    cairo_set_line_width(_Context.get(), width);
}

double surface::context::get_line_width() {
    return cairo_get_line_width(_Context.get());
}

void surface::context::set_miter_limit(double limit) {
    cairo_set_miter_limit(_Context.get(), limit);
}

double surface::context::get_miter_limit() {
    return cairo_get_miter_limit(_Context.get());
}

void surface::context::set_compositing_operator(compositing_operator co) {
    cairo_set_operator(_Context.get(), _Compositing_operator_to_cairo_operator_t(co));
}

compositing_operator surface::context::get_compositing_operator() {
    return _Cairo_operator_t_to_compositing_operator(cairo_get_operator(_Context.get()));
}

void surface::context::set_tolerance(double tolerance) {
    cairo_set_tolerance(_Context.get(), tolerance);
}

double surface::context::get_tolerance() {
    return cairo_get_tolerance(_Context.get());
}

void surface::context::clip() {
    cairo_clip(_Context.get());
}

void surface::context::clip_preserve() {
    cairo_clip_preserve(_Context.get());
}

void surface::context::clip_extents(double& x1, double& y1, double& x2, double& y2) {
    cairo_clip_extents(_Context.get(), &x1, &y1, &x2, &y2);
}

bool surface::context::in_clip(double x, double y) {
    return cairo_in_clip(_Context.get(), x, y) != 0;
}

void surface::context::reset_clip() {
    cairo_reset_clip(_Context.get());
}

rectangle_list surface::context::copy_clip_rectangle_list() {
    rectangle_list results;
    std::unique_ptr<cairo_rectangle_list_t, function<void(cairo_rectangle_list_t*)>> sp_rects(cairo_copy_clip_rectangle_list(_Context.get()), &cairo_rectangle_list_destroy);
    results.status = _Cairo_status_t_to_status(sp_rects->status);
    _Throw_if_failed_status(results.status);
    for (auto i = 0; i < sp_rects->num_rectangles; ++i) {
        results.rectangles.push_back({ sp_rects->rectangles[i].x, sp_rects->rectangles[i].y, sp_rects->rectangles[i].width, sp_rects->rectangles[i].height });
    }

    return results;
}

void surface::context::fill() {
    cairo_fill(_Context.get());
}

void surface::context::fill_preserve() {
    cairo_fill_preserve(_Context.get());
}

void surface::context::fill_extents(double& x1, double& y1, double& x2, double& y2) {
    cairo_fill_extents(_Context.get(), &x1, &y1, &x2, &y2);
}

bool surface::context::in_fill(double x, double y) {
    return cairo_in_fill(_Context.get(), x, y) != 0;
}

void surface::context::mask(pattern& pattern) {
    cairo_mask(_Context.get(), pattern.native_handle());
}

void surface::context::mask_surface(surface& surface, double surface_x, double surface_y) {
    cairo_mask_surface(_Context.get(), surface.native_handle(), surface_x, surface_y);
}

void surface::context::paint() {
    cairo_paint(_Context.get());
}

void surface::context::paint_with_alpha(double alpha) {
    cairo_paint_with_alpha(_Context.get(), alpha);
}

void surface::context::stroke() {
    cairo_stroke(_Context.get());
}

void surface::context::stroke_preserve() {
    cairo_stroke_preserve(_Context.get());
}

void surface::context::stroke_extents(double& x1, double& y1, double& x2, double& y2) {
    cairo_stroke_extents(_Context.get(), &x1, &y1, &x2, &y2);
}

bool surface::context::in_stroke(double x, double y) {
    return cairo_in_stroke(_Context.get(), x, y) != 0;
}

void surface::context::copy_page() {
    cairo_copy_page(_Context.get());
}

void surface::context::show_page() {
    cairo_show_page(_Context.get());
}

path surface::context::copy_path() {
    path result;
    unique_ptr<cairo_path_t, function<void(cairo_path_t*)>> sp_path(cairo_copy_path(_Context.get()), &cairo_path_destroy);

    _Throw_if_failed_status(_Cairo_status_t_to_status(sp_path->status));

    return _Make_path(sp_path.get());
}

path surface::context::copy_path_flat() {
    path result;
    unique_ptr<cairo_path_t, function<void(cairo_path_t*)>> sp_path(cairo_copy_path(_Context.get()), &cairo_path_destroy);

    _Throw_if_failed_status(_Cairo_status_t_to_status(sp_path->status));

    return _Make_path(sp_path.get());
}

void surface::context::append_path(const path& p) {
    cairo_path_t cairoPath = { };
    cairoPath.status = CAIRO_STATUS_SUCCESS;
    int numData = static_cast<int>(p.data.size());
    cairoPath.num_data = numData;
    vector<cairo_path_data_t> vec;
    cairo_path_data_t cpd = { };
    for (auto i = 0; i < numData; i += p.data[i].header.length) {
        cpd.header.length = p.data[i].header.length;
        cpd.header.type = _Path_data_type_to_cairo_path_data_type_t(p.data[i].header.type);
        vec.push_back(cpd);
        auto pdLength = p.data[i].header.length;
        // We already pushed the first element so j starts at 1 not 0.
        for (auto j = 1; j < pdLength; ++j) {
            cpd.point.x = p.data[i + j].point.x;
            cpd.point.y = p.data[i + j].point.y;
            vec.push_back(cpd);
        }
    }
    cairoPath.data = vec.data();
    cairo_append_path(_Context.get(), &cairoPath);
}

bool surface::context::has_current_point() {
    return cairo_has_current_point(_Context.get()) != 0;
}

void surface::context::get_current_point(double& x, double& y) {
    cairo_get_current_point(_Context.get(), &x, &y);
}

void surface::context::new_path() {
    cairo_new_path(_Context.get());
}

void surface::context::new_sub_path() {
    cairo_new_sub_path(_Context.get());
}

void surface::context::close_path() {
    cairo_close_path(_Context.get());
}

void surface::context::arc(double xc, double yc, double radius, double angle1, double angle2) {
    cairo_arc(_Context.get(), xc, yc, radius, angle1, angle2);
}

void surface::context::arc_negative(double xc, double yc, double radius, double angle1, double angle2) {
    cairo_arc_negative(_Context.get(), xc, yc, radius, angle1, angle2);
}

void  surface::context::curve_to(double x1, double y1, double x2, double y2, double x3, double y3) {
    cairo_curve_to(_Context.get(), x1, y1, x2, y2, x3, y3);
}

void surface::context::line_to(double x, double y) {
    cairo_line_to(_Context.get(), x, y);
}

void surface::context::move_to(double x, double y) {
    cairo_move_to(_Context.get(), x, y);
}

void surface::context::rectangle(double x, double y, double width, double height) {
    cairo_rectangle(_Context.get(), x, y, width, height);
}

void surface::context::glyph_path(const vector<glyph>& glyphs) {
    vector<cairo_glyph_t> vec;
    for (const auto& glyph : glyphs) {
        vec.push_back({ glyph.index, glyph.x, glyph.y });
    }
    cairo_glyph_path(_Context.get(), vec.data(), static_cast<int>(vec.size()));
}

void surface::context::text_path(const string& utf8) {
    cairo_text_path(_Context.get(), utf8.c_str());
}

void surface::context::rel_curve_to(double dx1, double dy1, double dx2, double dy2, double dx3, double dy3) {
    cairo_rel_curve_to(_Context.get(), dx1, dy1, dx2, dy2, dx3, dy3);
}

void surface::context::rel_line_to(double dx, double dy) {
    cairo_rel_line_to(_Context.get(), dx, dy);
}

void surface::context::rel_move_to(double dx, double dy) {
    cairo_rel_move_to(_Context.get(), dx, dy);
}

void surface::context::path_extents(double& x1, double& y1, double& x2, double& y2) {
    cairo_path_extents(_Context.get(), &x1, &y1, &x2, &y2);
}

void surface::context::translate(double tx, double ty) {
    cairo_translate(_Context.get(), tx, ty);
}

void surface::context::scale(double sx, double sy) {
    cairo_scale(_Context.get(), sx, sy);
}

void surface::context::rotate(double angle) {
    cairo_rotate(_Context.get(), angle);
}

void surface::context::transform(const matrix& m) {
    cairo_matrix_t cm{ m.xx, m.yx, m.xy, m.yy, m.x0, m.y0 };
    cairo_transform(_Context.get(), &cm);
}

void surface::context::set_matrix(const matrix& m) {
    cairo_matrix_t cm{ m.xx, m.yx, m.xy, m.yy, m.x0, m.y0 };
    cairo_set_matrix(_Context.get(), &cm);
}

void surface::context::get_matrix(matrix& matrix) {
    cairo_matrix_t cm{ };
    cairo_get_matrix(_Context.get(), &cm);
    matrix = { cm.xx, cm.yx, cm.xy, cm.yy, cm.x0, cm.y0 };
}

void surface::context::identity_matrix() {
    cairo_identity_matrix(_Context.get());
}

void surface::context::user_to_device(double& x, double& y) {
    cairo_user_to_device(_Context.get(), &x, &y);
}

void surface::context::user_to_device_distance(double& dx, double& dy) {
    cairo_user_to_device_distance(_Context.get(), &dx, &dy);
}

void surface::context::device_to_user(double& x, double& y) {
    cairo_device_to_user(_Context.get(), &x, &y);
}

void surface::context::device_to_user_distance(double& dx, double& dy) {
    cairo_device_to_user_distance(_Context.get(), &dx, &dy);
}

void surface::context::select_font_face(const string& family, font_slant slant, font_weight weight) {
    cairo_select_font_face(_Context.get(), family.c_str(), _Font_slant_to_cairo_font_slant_t(slant), _Font_weight_to_cairo_font_weight_t(weight));
}

void surface::context::set_font_size(double size) {
    cairo_set_font_size(_Context.get(), size);
}

void surface::context::set_font_matrix(const matrix& m) {
    cairo_matrix_t cm{ m.xx, m.yx, m.xy, m.yy, m.x0, m.y0 };
    cairo_set_font_matrix(_Context.get(), &cm);
}

void surface::context::get_font_matrix(matrix& matrix) {
    cairo_matrix_t cm{ };
    cairo_get_font_matrix(_Context.get(), &cm);
    matrix = { cm.xx, cm.yx, cm.xy, cm.yy, cm.x0, cm.y0 };
}

void surface::context::set_font_options(const font_options& options) {
    cairo_set_font_options(_Context.get(), options.native_handle());
}

void surface::context::get_font_options(font_options& options) {
    cairo_get_font_options(_Context.get(), options.native_handle());
}

void surface::context::set_font_face(font_face& font_face) {
    cairo_set_font_face(_Context.get(), font_face.native_handle());
}

font_face surface::context::get_font_face() {
    auto ff = cairo_get_font_face(_Context.get());
    _Throw_if_failed_status(_Cairo_status_t_to_status(cairo_font_face_status(ff)));
    // Cairo doesn't increase the font face's reference count when you call cairo_get_font_face so we do it manually.
    return font_face(cairo_font_face_reference(ff));
}

void surface::context::set_scaled_font(const scaled_font& scaled_font) {
    cairo_set_scaled_font(_Context.get(), scaled_font.native_handle());
}

scaled_font surface::context::get_scaled_font() {
    auto sf = cairo_get_scaled_font(_Context.get());
    _Throw_if_failed_status(_Cairo_status_t_to_status(cairo_scaled_font_status(sf)));
    // Cairo doesn't increase the scaled font's reference count when you call cairo_get_scaled_font so we do it manually.
    return scaled_font(cairo_scaled_font_reference(sf));
}

void surface::context::show_text(const string& utf8) {
    cairo_show_text(_Context.get(), utf8.c_str());
}

void surface::context::show_glyphs(const vector<glyph>& glyphs) {
    vector<cairo_glyph_t> vec;
    for (const auto& glyph : glyphs) {
        vec.push_back({ glyph.index, glyph.x, glyph.y });
    }
    cairo_show_glyphs(_Context.get(), vec.data(), static_cast<int>(vec.size()));
}

void surface::context::show_text_glyphs(const string& utf8, const vector<glyph>& glyphs, const vector<text_cluster>& clusters, text_cluster_flags::text_cluster_flags cluster_flags) {
    vector<cairo_glyph_t> vec;
    for (const auto& glyph : glyphs) {
        vec.push_back({ glyph.index, glyph.x, glyph.y });
    }
    const auto tcSize = static_cast<int>(clusters.size());
    unique_ptr<cairo_text_cluster_t, function<void(cairo_text_cluster_t*)>> sp_tc(cairo_text_cluster_allocate(tcSize), &cairo_text_cluster_free);
    auto tc_ptr = sp_tc.get();
    for (auto i = 0; i < tcSize; ++i) {
        tc_ptr[i].num_bytes = clusters[i].num_bytes;
        tc_ptr[i].num_glyphs = clusters[i].num_glyphs;
    }
    auto ctcf = _Text_cluster_flags_to_cairo_text_cluster_flags_t(cluster_flags);
    cairo_show_text_glyphs(_Context.get(), utf8.data(), static_cast<int>(utf8.size()), vec.data(), static_cast<int>(vec.size()), sp_tc.get(), tcSize, ctcf);
}

void surface::context::font_extents(experimental::drawing::font_extents& extents) {
    cairo_font_extents_t result{ };
    cairo_font_extents(_Context.get(), &result);
    extents.ascent = result.ascent;
    extents.descent = result.descent;
    extents.height = result.height;
    extents.max_x_advance = result.max_x_advance;
    extents.max_y_advance = result.max_y_advance;
}

void surface::context::text_extents(const string& utf8, experimental::drawing::text_extents& extents) {
    cairo_text_extents_t result{ };
    cairo_text_extents(_Context.get(), utf8.c_str(), &result);
    extents.height = result.height;
    extents.width = result.width;
    extents.x_advance = result.x_advance;
    extents.x_bearing = result.x_bearing;
    extents.y_advance = result.y_advance;
    extents.y_bearing = result.y_bearing;
}

void surface::context::glyph_extents(const vector<glyph>& glyphs, experimental::drawing::text_extents& extents) {
    vector<cairo_glyph_t> vec;
    for (const auto& glyph : glyphs) {
        vec.push_back({ glyph.index, glyph.x, glyph.y });
    }
    cairo_text_extents_t result{ };
    cairo_glyph_extents(_Context.get(), vec.data(), static_cast<int>(vec.size()), &result);
    extents.height = result.height;
    extents.width = result.width;
    extents.x_advance = result.x_advance;
    extents.x_bearing = result.x_bearing;
    extents.y_advance = result.y_advance;
    extents.y_bearing = result.y_bearing;
}
