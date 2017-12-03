classdef SonarWallX < SonarWall
    %SONARWALLX Class for sonar walls parallel to the x-axis.
    %   Created by RBE-2002 B17 Team 10.
    %   
    %   See also: SONARWALL, SONARWALLY
    
    properties (Access = public)
        yPos;   % y-position of wall (meters)
        xMin;   % minimum x-position (meters)
        xMax;   % maximum x-position (meters)
    end
    
    methods (Access = public)
        function obj = SonarWallX(pos)
            % Constructs x-wall from given position vector.
            % pos = Point to build wall from ([x; y])
            obj.yPos = pos(2);
            obj.xMin = pos(1);
            obj.xMax = pos(1);
        end
        function [f] = fitsPoint(obj, point)
            % Returns logical 1 if point could be added to wall.
            x = point(1);
            y = point(2);
            f = (abs(y - obj.yPos) < obj.normalLimit) && ...
                (x >= obj.xMin - obj.edgeLimit) && ...
                (x <= obj.xMax + obj.edgeLimit);
        end
        function [f] = onPositiveSide(obj, point)
            % Returns 1 if point is on +y side of wall and within x-bounds.
            x = point(1);
            y = point(2);
            f = (y > obj.yPos) && ...
                (x >= obj.xMin) && ...
                (x <= obj.xMax);
        end
        function [f] = onNegativeSide(obj, point)
            % Returns 1 if point is on -y side of wall and within x-bounds.
            x = point(1);
            y = point(2);
            f = (y < obj.yPos) && ...
                (x >= obj.xMin) && ...
                (x <= obj.xMax);
        end
        function addPoint(obj, point, side)
            % Expands wall by adding point ([x; y]) to its hypothesis.
            % Side is the side of the wall the point is on ('+y' or '-y').
            x = point(1);
            switch side
                case '+y', y = point(2) - obj.radius;
                case '-y', y = point(2) + obj.radius;
                otherwise, error('Side should be +y or -y');
            end
            
            % Include y position in average
            obj.yPos = (obj.points * obj.yPos + y) / (obj.points + 1);
            obj.points = obj.points + 1;
            
            % Expand wall in x-direction
            if x > obj.xMax
                obj.xMax = x;
            elseif x < obj.xMin
                obj.xMin = x;
            end
            
            % Reset dormancy
            obj.dormancy = 0;
        end
        function [len] = getLength(obj)
            % Returns length of wall in meters.
            len = obj.xMax - obj.xMin;
        end
        function plot(obj, plotFmt)
            % Plots wall on current axis.
            % plotFmt can be '--', '-bo', 'rx', etc.
            x0 = obj.xMin;
            x1 = obj.xMax;
            y0 = obj.yPos - obj.radius;
            y1 = obj.yPos + obj.radius;
            x = [x0 x1 x1 x0 x0];
            y = [y0 y0 y1 y1 y0];
            plot(x, y, plotFmt)
        end
    end
end