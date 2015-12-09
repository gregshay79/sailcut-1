/*
 * Copyright (C) 1993-2015 Robert & Jeremy Laine
 * See AUTHORS file for a full list of contributors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <geocpp/geometry.h>
#include <geocpp/matrix.h>
#include <geocpp/subspace.h>


/**
 * Returns true if v1 and v2 are equal, allowing for a small fuzziness factor
 * for floating-point comparisons; false otherwise.
 */
bool qFuzzyCompare(const CVector3d &v1, const CVector3d &v2)
{
    for (size_t i = 0; i < 3; i++)
        if (fabs(v1[i] - v2[i]) > EPS)
            return false;
    return true;
}

CVector3d operator*(const QMatrix4x4 &m, const CVector3d &v)
{
    QVector3D o = m * QVector3D(v.x(), v.y(), v.z());
    return CVector3d(o.x(), o.y(), o.z());
}


/** Cross product. Produces a vector orthogonal to the two original vectors.
 */
CVector3d CVector3d::crossProduct(const CVector3d &v1, const CVector3d &v2)
{
    CVector3d ret;
    for (int i = 0; i < 3; i++)
        ret[i] = v1[(i+1)%3] * v2[(i+2)%3] - v1[(i+2)%3] * v2[(i+1)%3];
    return ret;
}

/** Dot (real) product.
 */
real CVector3d::dotProduct(const CVector3d &v1, const CVector3d &v2)
{
    real ret = 0;
    for (size_t i = 0; i < 3; i++)
        ret += v1[i] * v2[i];
    return ret;
}

/***************************************

                 3D box

***************************************/

/** Translate box by a vector.
 *
 * @param transl translation vector
 */
CRect3d CRect3d::operator+ (const CVector3d &transl) const
{
    CRect3d ret;
    ret.min = min + transl;
    ret.max = max + transl;
    return ret;
}


/** Expand/shrink box by a coefficient, keeping the same center.
 *
 * @param r coefficient
 */
CRect3d CRect3d::operator* (const real r) const
{
    CRect3d ret;
    CVector3d off = (r-1) * 0.5 * (max - min);
    ret.min = min - off;
    ret.max = max + off;
    return ret;
}


/** Return the minimum rectangle containing the current rectangle
 *  with the given aspect ratio (width / height).
 */
CRect3d CRect3d::expandToRatio(const real ratio) const
{
    CRect3d lRect(*this);
    const real w = width();
    const real h = height();

    if (!w || !h)
        return lRect;


    const real objAspect = w / h;
    if (objAspect > ratio)
    {
        const real extrah = 0.5 * h * (objAspect/ratio - 1);
        // we are limited by the width of the window, grow logical viewport's height
        lRect.min.setY(lRect.min.y() - extrah);
        lRect.max.setY(lRect.max.y() + extrah);
    }
    else
    {
        real extraw = 0.5 * w * (ratio/objAspect - 1);
        // we are limited by the height of the window, grow logical viewport's width
        lRect.min.setX(lRect.min.x() - extraw);
        lRect.max.setX(lRect.max.x() + extraw);
    }

    return lRect;
}


/** Return the minimum rectangle containing the current rectangle
 *  and the one given as an argument.
 *
 * @param rect
 */
CRect3d CRect3d::join(const CRect3d& rect) const
{
    CRect3d ret = *this;
    for (size_t j = 0; j < 3; j++)
    {
        if (rect.min[j] < ret.min[j])
            ret.min[j] = rect.min[j];
        if (rect.max[j] > ret.max[j])
            ret.max[j] = rect.max[j];
    }
    return ret;
}

