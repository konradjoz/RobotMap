#include "include/clib.h"

class PyStdErrOutStreamRedirect {
    py::object _stdout;
    py::object _stderr;
    py::object _stdout_buffer;
    py::object _stderr_buffer;
public:
    PyStdErrOutStreamRedirect() {
        auto sysm = py::module::import("sys");
        _stdout = sysm.attr("stdout");
        _stderr = sysm.attr("stderr");
        auto stringio = py::module::import("io").attr("StringIO");
        _stdout_buffer = stringio();  // Other filelike object can be used here as well, such as objects created by pybind11
        _stderr_buffer = stringio();
        sysm.attr("stdout") = _stdout_buffer;
        sysm.attr("stderr") = _stderr_buffer;
    }
    std::string stdoutString() {
        _stdout_buffer.attr("seek")(0);
        return py::str(_stdout_buffer.attr("read")());
    }
    std::string stderrString() {
        _stderr_buffer.attr("seek")(0);
        return py::str(_stderr_buffer.attr("read")());
    }
    ~PyStdErrOutStreamRedirect() {
        auto sysm = py::module::import("sys");
        sysm.attr("stdout") = _stdout;
        sysm.attr("stderr") = _stderr;
    }
};

void map(float robomap[][3])
{
    PyStdErrOutStreamRedirect pyOutputRedirect{};
    float lastx = 0.0;
    float lasty = 0.0;
    float biggestx = 0.0;
    float biggesty = 0.0;
    int k = 19;
    int map[101][101]{0};
    // variable initialization
    for (int i = 1; i <= k; i++)
    {
        lastx = lastx + (robomap[0][i] - robomap[0][i - 1]) * cos((robomap[1][i] * 3.14) / 180.0);
        lasty = lasty + (robomap[0][i] - robomap[0][i - 1]) * sin((robomap[1][i] * 3.14) / 180.0);
        if (abs(lastx) > biggestx)
        {
            biggestx = abs(lastx);
        }
        if (abs(lasty) > biggesty)
        {
            biggesty = abs(lasty);
        }
    }
    float scale = 0;
    if (biggestx > biggesty)
    {
        scale = (biggestx + 900) / 50.0;
    }
    else
    {
        scale = (biggesty + 900) / 50.0;
    }
    // scaling shit works uh don't change it
    printf("Each square is %f Cm across\n", scale);
    int startx = 10;
    int starty = 10;
    map[10][10] = 1;
    // initializes in the middle of the map
    for (int i = 1; i < k; i++)
    {
        float angle = ((robomap[1][i] * 3.14) / 180.0);
        float lengthhyp = robomap[0][i] - robomap[0][i - 1];
        int endpointx = startx + round(cos(angle) * lengthhyp / scale);
        int endpointy = starty + round(sin(angle) * lengthhyp / scale);
        int pointx = startx;
        int pointy = starty;
        // loops changing x and y to fit slope until it reaches the end point
        //  this is used to draw a section of it's journey before it turns then goes on it's next section which is why it loops
        while ((pointx != endpointx) || (pointy != endpointy))
        {
            map[pointx][pointy] = 1;
            // paints current position green
            if ((angle > 0 * 3.14 / 180) && (angle < 90 * 3.14 / 180))
            {
                // between 0 and 90 degrees checks x and y against rounded slope and end point to see if one or both need to be increased
                if (pointx < endpointx && pointx < startx + round((pointy - starty + 1) / tan(angle)))
                {
                    pointx++;
                }
                if (pointy < starty + round((pointx - startx + 1) * tan(angle)) && (pointy < endpointy))
                {
                    pointy++;
                }
            }
            else if ((angle > 90 * 3.14 / 180) && (angle < 180 * 3.14 / 180))
            {
                // between 90 and 180 degrees checks x and y against rounded slope and end point to see if x needs to be decreased and y needs to be increased
                if (pointx > endpointx && pointx > startx + round((pointy - starty + 1) / tan(angle)))
                {
                    pointx--;
                }
                if (pointy < starty + round((pointx - startx - 1) * tan(angle)) && (pointy < endpointy))
                {
                    pointy++;
                }
            }
            else if ((angle > 180 * 3.14 / 180) && (angle < 270 * 3.14 / 180))
            {
                // between 180 and 270 degrees checks x and y against rounded slope and end point to see if x needs to be decreased and y needs to be decreased
                if (pointx > endpointx && pointx < startx - round((pointy - starty - 1) / tan(angle)))
                {
                    pointx--;
                }
                if (pointy > starty + round((pointx - startx - 1) * tan(angle)) && (pointy > endpointy))
                {
                    pointy--;
                }
            }
            else
            {
                // between 270 and 360 degrees checks x and y against rounded slope and end point to see if x needs to be increased and y needs to be decreased
                if (pointx < endpointx && pointx < startx + round((pointy - starty - 1) / tan(angle)))
                {
                    pointx++;
                }
                if (pointy < starty + round((pointx - startx - 1) * tan(angle)) && (pointy > endpointy))
                {
                    pointy--;
                }
            }
        }
        // sets start position to end of line segment
        startx = pointx;
        starty = pointy;
        map[pointx][pointy] = 1;
        // paints current position green
        lengthhyp = robomap[2][i];
        // sets sensor dist to next line segment

        if (lengthhyp == 0)
        {
            // if bumper was pressed makes current square an obstacle
            map[pointx][pointy] = 2;
        }
        else
        {
            endpointx = startx + round(cos(angle) * lengthhyp / scale);
            endpointy = starty + round(sin(angle) * lengthhyp / scale);
            pointx = startx;
            pointy = starty;
            // loops changing x and y to fit slope until it reaches the end point
            //  this part is used to draw a straight line segment to the sensor data point ending in an obstacle square (red)
            while ((pointx != endpointx) || (pointy != endpointy))
            {
                map[pointx][pointy] = 1;
                // paints current position green
                if ((angle > 0 * 3.14 / 180) && (angle < 90 * 3.14 / 180))
                {
                    // between 0 and 90 degrees checks x and y against rounded slope and end point to see if one or both need to be increased
                    if (pointx < endpointx && pointx < startx + round((pointy - starty + 1) / tan(angle)))
                    {
                        pointx++;
                    }
                    if (pointy < starty + round((pointx - startx + 1) * tan(angle)) && (pointy < endpointy))
                    {
                        pointy++;
                    }
                }
                else if ((angle > 90 * 3.14 / 180) && (angle < 180 * 3.14 / 180))
                {
                    // between 90 and 180 degrees checks x and y against rounded slope and end point to see if x needs to be decreased and y needs to be increased
                    if (pointx > endpointx && pointx > startx + round((pointy - starty + 1) / tan(angle)))
                    {
                        pointx--;
                    }
                    if (pointy < starty + round((pointx - startx - 1) * tan(angle)) && (pointy < endpointy))
                    {
                        pointy++;
                    }
                }
                else if ((angle > 180 * 3.14 / 180) && (angle < 270 * 3.14 / 180))
                {
                    // between 180 and 270 degrees checks x and y against rounded slope and end point to see if x needs to be decreased and y needs to be decreased
                    if (pointx > endpointx && pointx < startx - round((pointy - starty - 1) / tan(angle)))
                    {
                        pointx--;
                    }
                    if (pointy > starty + round((pointx - startx - 1) * tan(angle)) && (pointy > endpointy))
                    {
                        pointy--;
                    }
                }
                else
                {
                    // between 270 and 360 degrees checks x and y against rounded slope and end point to see if x needs to be increased and y needs to be decreased
                    if (pointx < endpointx && pointx < startx + round((pointy - starty - 1) / tan(angle)))
                    {
                        pointx++;
                    }
                    if (pointy < starty + round((pointx - startx - 1) * tan(angle)) && (pointy > endpointy))
                    {
                        pointy--;
                    }
                }
            }
            // makes end of sensor line an obstacle
            map[pointx][pointy] = 2;
        }
    }
    // printing part also works don't change it unless color printing doesn't work then replace standard cout with printf and same codes probably
    for (int my = 0; my < 21; my++)
    {
        for (int mx = 0; mx < 21; mx++)
        {
            if (map[my][mx] == 1)
            {
                py::print("X");
            }
            else if (map[my][mx] == 2)
            {
                py::print("Y");
            }
            else
            {
                py::print("Z");
            }
        }
        std::cout << "\n";
    }
}