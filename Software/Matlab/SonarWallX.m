classdef SonarWallX < SonarWall
    %SONARWALLX Class for sonar walls parallel to the x-axis.
    %   Created by RBE-2002 B17 Team 10.
    %   
    %   See also: SONARWALL, SONARWALLY
    
    properties (Access = public) % MAKE PRIVATE POST DEBUG
        yPos;   % y-position of wall (meters)
        xMin;   % minimum x-position (meters)
        xMax;   % maximum x-position (meters)
    end
    
    methods
        function obj = SonarWallX(pos)
            % Constructs x-wall from given position vector.
            % pos = Point to build wall from ([x; y])
            obj.yPos = pos(2);
            obj.xMin = pos(1);
            obj.xMax = pos(1);
        end
        function [f] = fitsPoint(obj, point)
            % Returns logical 1 if point ([x; y]) fits in wall boundaries.
            x = point(1);
            y = point(2);
            f = (abs(y - obj.yPos) < obj.normalLimit) && ...
                (x > obj.xMin - obj.edgeLimit) && ...
                (x < obj.xMax + obj.edgeLimit);
        end
        function [point] = addPoint(obj, point)
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
        end
        function plot(obj)
            % Plots wall as solid blue line on current axes.
            x = [obj.xMin obj.xMax];
            y = [obj.yPos obj.yPos];
            plot(x, y, 'color', 'b')
            plot(x, y, 'x', 'color', 'b')
        end
    end
end