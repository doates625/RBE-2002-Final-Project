classdef MapBuilder < handle
    %MAPBUILDER Class for building field map for RBE-2002 final project.
    %   Created by RBE-2002 B17 Team 10.
    %   
    %   This class contains a list of SonarWallX and SonarWallY which
    %   together form the field map.
    %   
    %   All input points are in [x; y] format.
    %   
    %   See also: SONARWALL, SONARWALLX, SONARWALLY, ROBOTDATA
    
    properties (Access = private)
        xWalls = SonarWallX.empty;      % All walls parallel to x-axis.
        yWalls = SonarWallY.empty;      % All walls parallel to y-axis.
    end
    
    methods (Access = public)
        function update(obj, rd)
            % Incorporates RobotData 'rd' into new map approximation.
            switch alignment
                case {'+x', '-x'}
                    switch alignment 
                        case '+x'
                            sign1 = '+';
                            sign2 = '-';
                        case '-x'
                            sign1 = '-';
                            sign2 = '+';
                    end
                    if rd.sFvalid, obj.addToYWalls(rd.sonarF, [sign2 'x']); end
                    if rd.sBvalid, obj.addToYWalls(rd.sonarB, [sign1 'x']); end
                    if rd.sLvalid, obj.addToXWalls(rd.sonarL, [sign2 'y']); end
                    if rd.sRvalid, obj.addToXWalls(rd.sonarR, [sign1 'y']); end
                    
                case {'+y', '-y'}
                    switch alignment 
                        case '+y'
                            sign1 = '+';
                            sign2 = '-';
                        case '-y'
                            sign1 = '-';
                            sign2 = '+';
                    end
                    if rd.sFvalid, obj.addToXWalls(rd.sonarF, [sign2 'y']); end
                    if rd.sBvalid, obj.addToXWalls(rd.sonarB, [sign1 'y']); end
                    if rd.sLvalid, obj.addToYWalls(rd.sonarL, [sign2 'x']); end
                    if rd.sRvalid, obj.addToYWalls(rd.sonarR, [sign1 'x']); end
            end
            
            % Age x-walls and remove mistakes
            remove = [];
            for i = 1:length(obj.xWalls)
                obj.xWalls(i).incrementAge();
                if obj.xWalls(i).isMistake()
                    remove(end+1) = i;
                end
            end
            obj.xWalls(remove) = [];
            
            % Age y-walls and remove mistakes
            remove = [];
            for i = 1:length(obj.yWalls)
                obj.yWalls(i).incrementAge();
                if obj.yWalls(i).isMistake()
                    remove(end+1) = i;
                end
            end
            obj.yWalls(remove) = [];            
        end
        function plot(obj)
            % Plots all walls currently in the map on the current axes.
            for i = 1:length(obj.xWalls)
                obj.xWalls(i).plot('-b');
            end
            for i = 1:length(obj.yWalls)
                obj.yWalls(i).plot('-r');
            end
        end
    end
    methods (Access = private)
        function addToXWalls(obj, point, side)
            % Adds point to one of the x-walls or creates a new one if
            % point doesn't fit in any existing x-walls.
            % Side is side of the wall point is on ('+y' or '-y').
            for i = 1:length(obj.xWalls)
                if obj.xWalls(i).fitsPoint(point)
                    obj.xWalls(i).addPoint(point, side);
                    return
                end
            end
            obj.xWalls(end+1) = SonarWallX(point);
        end
        function addToYWalls(obj, point, side)
            % Adds point to one of the y-walls or creates a new one if
            % point doesn't fit in any existing y-walls.
            % Side is side of the wall point is on ('+x' or '-x').
            for i = 1:length(obj.yWalls)
                if obj.yWalls(i).fitsPoint(point)
                    obj.yWalls(i).addPoint(point, side);
                    return
                end
            end
            obj.yWalls(end+1) = SonarWallY(point);
        end
    end
end