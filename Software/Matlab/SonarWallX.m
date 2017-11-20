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
            % Returns logical 1 if point ([x; y]) fits in wall boundaries.         
            f = (abs(point(2) - obj.yPos) < obj.normalLimit) && ...
                obj.inXBounds(point(1));
        end
        function [f] = onPositiveSide(obj, point)
            % Returns 1 if point is on +y side of wall and within x-bounds.
            f = (point(2) >= obj.yPos) && obj.inXBounds(point(1));
        end
        function [f] = onNegativeSide(obj, point)
            % Returns 1 if point is on -y side of wall and within x-bounds.
            f = (point(2) < obj.yPos) && obj.inXBounds(point(1));
        end
        function addPoint(obj, point)
            % Expands wall by adding point ([x; y]) to its hypothesis.
            x = point(1);
            y = point(2);
            
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
            x = [obj.xMin obj.xMax];
            y = [obj.yPos obj.yPos];
            plot(x, y, plotFmt)
        end
    end
    methods (Access = private)
        function [f] = inXBounds(obj, x)
            % Returns true if x fits in wall x-boundaries.
            f = (x > obj.xMin - obj.edgeLimit) && ...
                (x < obj.xMax + obj.edgeLimit);
        end
    end
end