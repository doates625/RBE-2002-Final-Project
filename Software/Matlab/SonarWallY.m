classdef SonarWallY < SonarWall
    %SONARWALLY Class for sonar walls parallel to the y-axis.
    %   Created by RBE-2002 B17 Team 10.
    %   
    %   See also: SONARWALL, SONARWALLX
    
    properties (Access = public)
        xPos;   % x-position of wall (meters)
        yMin;   % minimum y-position (meters)
        yMax;   % maximum y-position (meters)
    end
    
    methods (Access = public)
        function obj = SonarWallY(pos)
            % Constructs y-wall from given position vector.
            % pos = Point to build wall from ([x; y])
            obj.xPos = pos(1);
            obj.yMin = pos(2);
            obj.yMax = pos(2);
        end
        function [f] = fitsPoint(obj, point)
            % Returns logical 1 if point ([x; y]) fits in wall boundaries.
            f = (abs(point(1) - obj.xPos) < obj.normalLimit) && ...
                obj.inYBounds(point(2));
                
        end
        function [f] = onPositiveSide(obj, point)
            % Returns 1 if point is on +x side of wall and within y-bounds.
            f = (point(1) > obj.xPos) && obj.inYBounds(point(2));
        end
        function [f] = onNegativeSide(obj, point)
            % Returns 1 if point is on -x side of wall and within y-bounds.
            f = (point(1) < obj.xPos) && obj.inYBounds(point(2));
        end
        function addPoint(obj, point)
            % Expands wall by adding point ([x; y]) to its hypothesis.
            x = point(1);
            y = point(2);
            
            % Include x position in average
            obj.xPos = (obj.points * obj.xPos + x) / (obj.points + 1);
            obj.points = obj.points + 1;
            
            % Expand wall in y-direction
            if y > obj.yMax
                obj.yMax = y;
            elseif y < obj.yMin
                obj.yMin = y;
            end
            
            % Reset dormancy
            obj.dormancy = 0;
        end       
        function [len] = getLength(obj)
            % Returns length of wall in meters.
            len = obj.yMax - obj.yMin;
        end
        function plot(obj, plotFmt)
            % Plots wall on current axis.
            % plotFmt can be '--', '-bo', 'rx', etc.
            x = [obj.xPos obj.xPos];
            y = [obj.yMin obj.yMax];
            plot(x, y, plotFmt)
        end
    end
    methods (Access = private)
        function [f] = inYBounds(obj, y)
            % Returns true if y fits in wall y-boundaries
            f = (y > obj.yMin - obj.edgeLimit) && ...
                (y < obj.yMax + obj.edgeLimit);
        end
    end
end