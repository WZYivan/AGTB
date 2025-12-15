#ifndef __AGTB_GEODESY_PROJECTION_GAUSS_KRUGER_HPP__
#define __AGTB_GEODESY_PROJECTION_GAUSS_KRUGER_HPP__

#include "../Datum.hpp"
#include "../SpatialReference/Geo/Geodetic.hpp"
#include "../SpatialReference/Proj/GaussKruger.hpp"

AGTB_GEODESY_BEGIN

namespace Projection::GaussKruger
{
    using SpatialRef::Proj::GaussKruger::GaussProjCoeffSolver;

    template <Ellipsoids __ellipsoid, GaussZoneInterval __zone_interval, Units __unit>
    GaussProjCoordinate<__zone_interval> GeodeticToGaussProj(const GeodeticCoordinate<__ellipsoid, __unit> &gc, int custom_zone = 0)
    {
        constexpr Ellipsoids ellipsoid = __ellipsoid;
        constexpr GaussZoneInterval zone_interval = __zone_interval;
        constexpr Units unit = __unit;
        using ellipsoid_geometry = EllipsoidGeometry<ellipsoid>;
        using coeff_solver = GaussProjCoeffSolver<__zone_interval, __unit>;

        Longitude<unit> L = gc.L;
        Latitude<unit> B = gc.B;
        LatitudeConstants<ellipsoid_geometry> glc(B);

        double t = glc.t,
               t2 = gcem::pow(t, 2),
               t4 = gcem::pow(t, 4),
               n2 = glc.nu_2,
               n4 = gcem::pow(n2, 2);
        double p = rho<Units::Second>,
               p2 = gcem::pow(p, 2),
               p3 = gcem::pow(p, 3),
               p4 = gcem::pow(p, 4),
               p5 = gcem::pow(p, 5),
               p6 = gcem::pow(p, 6);
        double sinB = B.Sin(),
               cosB = B.Cos(),
               cosBp3 = gcem::pow(cosB, 3),
               cosBp5 = gcem::pow(cosB, 5);
        int zone = custom_zone == 0 ? coeff_solver::Zone(L) : custom_zone;
        double l_c = coeff_solver::CenterLongitude(zone).Rad() /*rad*/,
               l_c_s = ToSeconds(l_c) /*seconds below*/,
               l_s = ToSeconds(L.Rad()),
               dl_s = l_s - l_c_s,
               l = dl_s,
               l2 = gcem::pow(dl_s, 2),
               l3 = gcem::pow(dl_s, 3),
               l4 = gcem::pow(dl_s, 4),
               l5 = gcem::pow(dl_s, 5),
               l6 = gcem::pow(dl_s, 6);
        auto [_, N] = PrincipleCurvatureRadii<ellipsoid, unit>(B);
        double X = MeridianArcLength<ellipsoid, unit>(B);

        double x = X +
                   N / (2 * p2) * sinB * cosB * l2 +
                   N / (24 * p4) * sinB * cosBp3 * (5 - t2 + 9 * n2 + 4 * n4) * l4 +
                   N / (720 * p6) * sinB * cosBp5 * (61 - 58 * t2 + t4) * l6;
        double y = N / p * cosB * l +
                   N / (6 * p3) * cosBp3 * (1 - t2 + n2) * l3 +
                   N / (120 * p5) * cosBp5 * (5 - 18 * t2 + t4 + 14 * n2 - 58 * n2 * t2) * l5;
        return {.x = x, .y = y, .zone = zone};
    }

    template <Ellipsoids __ellipsoid, GaussZoneInterval __zone_interval, Units __unit>
    GeodeticCoordinate<__ellipsoid, __unit> GaussProjToGeodetic(const GaussProjCoordinate<__zone_interval> &gpc)
    {
        constexpr Ellipsoids ellipsoid = __ellipsoid;
        constexpr GaussZoneInterval zone_interval = __zone_interval;
        constexpr Units unit = __unit;
        using ellipsoid_geometry = EllipsoidGeometry<ellipsoid>;

        double x = gpc.x, y = gpc.y;
        double zone = gpc.zone;
        Latitude<unit> Bf = MeridianArcBottom<ellipsoid, unit>(x, 1e-5, true);
        auto [Mf, Nf] = PrincipleCurvatureRadii<ellipsoid, unit>(Bf);
        LatitudeConstants<ellipsoid_geometry> glc(Bf);

        double tf = glc.t, nf2 = glc.nu_2;
        double tf2 = gcem::pow(tf, 2),
               tf4 = gcem::pow(tf, 4),
               nf4 = gcem::pow(nf2, 2);
        double Nf3 = gcem::pow(Nf, 3),
               Nf5 = gcem::pow(Nf, 5);
        double cosBf = Bf.Cos();
        double y2 = gcem::pow(y, 2),
               y3 = gcem::pow(y, 3),
               y4 = gcem::pow(y, 4),
               y5 = gcem::pow(y, 5),
               y6 = gcem::pow(y, 6);

        double B = Bf.Rad() -
                   tf / (2.0 * Mf * Nf) * y2 +
                   tf / (24 * Mf * Nf3) * (5 + 3 * tf2 + nf2 - 9 * nf2 * tf2) * y4 -
                   tf / (720 * Mf * Nf5) * (61 + 90 * tf2 + 45 * tf4) * y6;
        double dl = 1.0 / (Nf * cosBf) * y -
                    1.0 / (6.0 * Nf3 * cosBf) * (1 + 2 * tf2 + nf2) * y3 +
                    1.0 / (120 * Nf5 * cosBf) * (5 + 28 * tf2 + 24 * tf4 + 6 * nf2 + 8 * nf2 * tf2) * y5;
        Longitude<unit> Lc = GaussProjCoeffSolver<__zone_interval, __unit>::CenterLongitude(zone);

        return {
            .L = Longitude<unit>(Lc.Rad() + dl),
            .B = Latitude<unit>(B)};
    }

    template <Ellipsoids __ellipsoid, GaussZoneInterval __zone_interval, Units __unit>
    GaussProjCoordinate<__zone_interval> TransformZone(const GaussProjCoordinate<__zone_interval> &src, int tar_zone)
    {
        using GaussCoord = GaussProjCoordinate<__zone_interval>;
        using GeoCoord = GeodeticCoordinate<__ellipsoid, __unit>;

        GeoCoord geo_coord = GaussProjToGeodetic<__ellipsoid, __zone_interval, __unit>(src);
        return GeodeticToGaussProj<__ellipsoid, __zone_interval, __unit>(geo_coord, tar_zone);
    }

    template <Ellipsoids __ellipsoid, GaussZoneInterval __zone_interval, Units __unit>
    Angle MeridianConvergence(const GeodeticCoordinate<__ellipsoid, __unit> &gc)
    {
        auto L = gc.L;
        auto B = gc.B;

        Longitude<__unit> Lc = GaussProjCoeffSolver<__zone_interval, __unit>::CenterLongitude(L);
        double
            l = L.Rad() - Lc.Rad(),
            l3 = gcem::pow(l, 3),
            l5 = gcem::pow(l, 5),
            sinB = B.Sin(),
            cosB = B.Cos(),
            cosBp2 = gcem::pow(cosB, 2),
            cosBp4 = gcem::pow(cosB, 4);
        using ellipsoid_geometry = EllipsoidGeometry<__ellipsoid>;
        LatitudeConstants<ellipsoid_geometry> lc(B);
        double
            n2 = lc.nu_2,
            n4 = gcem::pow(n2, 2),
            t = lc.t,
            t2 = gcem::pow(t, 2);
        double gamma =
            sinB * l +
            1.0 / 3.0 * sinB * cosBp2 * l3 * (1 + 3 * n2 + 2 * n4) +
            1.0 / 15.0 * sinB * cosBp4 * l5 * (2 - t2);
        return Angle::FromRad(gamma);
    }

    template <Ellipsoids __ellipsoid, GaussZoneInterval __zone_interval, Units __unit>
    Angle MeridianConvergence(const GaussProjCoordinate<__zone_interval> &gpc)
    {
        double x = gpc.x, y = gpc.y;
        Latitude<__unit> Bf = MeridianArcBottom<__ellipsoid, __unit>(x);
        using ellipsoid_geometry = EllipsoidGeometry<__ellipsoid>;
        LatitudeConstants<ellipsoid_geometry> lc(Bf);
        double
            t = lc.t,
            t2 = gcem::pow(t, 2),
            t4 = gcem::pow(t, 4),
            n2 = lc.nu_2;
        CurvatureRadiusCollection crc = PrincipleCurvatureRadii<__ellipsoid, __unit>(Bf);
        double
            N = crc.N,
            N3 = gcem::pow(N, 3),
            N5 = gcem::pow(N, 5),
            y3 = gcem::pow(y, 3),
            y5 = gcem::pow(y, 5);
        double gamma =
            y * t / N -
            y3 / (3 * N3) * t * (1 + t2 - n2) +
            y5 / (15 * N5) * t * (2 + 5 * t2 + 3 * t4);
        return Angle::FromRad(gamma);
    }

    struct DirctionCorrectionResult
    {
        Angle delta_forward, delta_backward;
    };

    template <Ellipsoids __ellipsoid, GaussZoneInterval __zone_interval, Units __unit>
    DirctionCorrectionResult DirectionCorrection(const GaussProjCoordinate<__zone_interval> &gpc1, const GaussProjCoordinate<__zone_interval> &gpc2)
    {
        GeodeticCoordinate<__ellipsoid, __unit>
            gc1 = GaussProjToGeodetic<__ellipsoid, __zone_interval, __unit>(gpc1),
            gc2 = GaussProjToGeodetic<__ellipsoid, __zone_interval, __unit>(gpc2);
        double
            x1 = gpc1.x,
            y1 = gpc1.y,
            x2 = gpc2.x,
            y2 = gpc2.y,
            ym = (y1 + y2) / 2.0,
            ym2 = gcem::pow(ym, 2),
            ym3 = gcem::pow(ym, 3);

        Latitude<__unit>
            B1 = gc1.B,
            B2 = gc2.B,
            Bm((B1.Rad() + B2.Rad()) / 2.0);
        CurvatureRadiusCollection crc = PrincipleCurvatureRadii<__ellipsoid, __unit>(Bm);
        double
            Rm = crc.R(),
            Rm2 = gcem::pow(Rm, 2),
            Rm3 = gcem::pow(Rm, 3);
        LatitudeConstants<EllipsoidGeometry<__ellipsoid>> lc(Bm);
        double n2 = lc.nu_2, t = lc.t;

        double
            delta_forward =
                -(x2 - x1) / (6 * Rm2) * (2 * y1 + y2 - ym3 / Rm2) -
                n2 * t / Rm3 * (y2 - y1) * ym2,
            delta_backward =
                (x2 - x1) / (6 * Rm2) * (2 * y2 + y1 - ym3 / Rm2) +
                n2 * t / Rm3 + (y2 - y1) * ym2;
        return {
            Angle::FromRad(delta_forward),
            Angle::FromRad(delta_backward)};
    }

    template <Ellipsoids __ellipsoid, GaussZoneInterval __zone_interval, Units __unit>
    double Strech(const GeodeticCoordinate<__ellipsoid, __unit> &gc)
    {
        Longitude<__unit> lc = GaussProjCoeffSolver<__zone_interval, __unit>::CenterLongitude(gc.L);
        Longitude<__unit> dl(lc.Rad() - gc.L.Rad());
        double
            l = dl.Rad(),
            l2 = gcem::pow(l, 2),
            l4 = gcem::pow(l, 4),
            cosB = gc.B.Cos(),
            cosBp2 = gcem::pow(cosB, 2),
            cosBp4 = gcem::pow(cosB, 4);
        LatitudeConstants<EllipsoidGeometry<__ellipsoid>> lat_const(gc.B);
        double
            n2 = lat_const.nu_2,
            t = lat_const.t,
            t2 = gcem::pow(t, 2);
        double m =
            1 +
            1.0 / 2.0 * l2 * cosBp2 * (1 + n2) +
            1.0 / 24.0 * l4 * cosBp4 * (5 - 4 * t2);
        return m;
    }

    template <Ellipsoids __ellipsoid, GaussZoneInterval __zone_interval, Units __unit>
    double Strech(const GaussProjCoordinate<__zone_interval> &gpc)
    {
        GeodeticCoordinate<__ellipsoid, __unit> gc = GaussProjToGeodetic<__ellipsoid, __zone_interval, __unit>(gpc);
        CurvatureRadiusCollection crc = PrincipleCurvatureRadii<__ellipsoid, __unit>(gc.B);
        double
            R = crc.R(),
            R2 = gcem::pow(R, 2),
            R4 = gcem::pow(R, 4),
            y = gpc.y,
            y2 = gcem::pow(y, 2),
            y4 = gcem::pow(y, 4),
            m = 1.0 +
                y2 / (2 * R2) +
                y4 / (24 * R4);
        return m;
    }

    template <Ellipsoids __ellipsoid, GaussZoneInterval __zone_interval, Units __unit>
    double DistanceCorrection(double S, const GeodeticCoordinate<__ellipsoid, __unit> &beg, const GeodeticCoordinate<__ellipsoid, __unit> &end)
    {
        GaussProjCoordinate<__zone_interval>
            proj_beg = GeodeticToGaussProj<__ellipsoid, __zone_interval, __unit>(beg),
            proj_end = GeodeticToGaussProj<__ellipsoid, __zone_interval, __unit>(end);
        Latitude<__unit> Bm((beg.B.Rad() + end.B.Rad()) / 2.0);
        CurvatureRadiusCollection crc = PrincipleCurvatureRadii<__ellipsoid, __unit>(Bm);
        double
            Rm = crc.R(),
            Rm2 = gcem::pow(Rm, 2),
            Rm4 = gcem::pow(Rm, 4),
            ym = (proj_beg.y + proj_end.y) / 2.0,
            ym2 = gcem::pow(ym, 2),
            ym4 = gcem::pow(ym, 4),
            dy = proj_end.y - proj_beg.y,
            dy2 = gcem::pow(dy, 2),
            scale = 1.0 +
                    ym2 / (2 * Rm2) +
                    ym4 / (24 * Rm4) +
                    dy2 / (24 * Rm2),
            D = scale * S;
        return D;
    }
}

AGTB_GEODESY_END

#endif