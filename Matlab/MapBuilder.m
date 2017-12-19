classdef MapBuilder < handle
    %MAPBUILDER Class for building field map for RBE-2002 final project.
    %   Created by Dan Oates (RBE-2002 B17 Team 10).
    %   
    %   This class contains a list of SonarWallX and SonarWallY which
    %   together form the field map.
    %   
    %   All input points are in [x; y] format.
    %   
    %   See also: SONARWALL, SONARWALLX, SONARWALLY, ROBOTDATA
    
    properties (Access = private)
        xWalls = SonarWallX.empty;  % All walls parallel to x-axis.
        yWalls = SonarWallY.empty;  % All walls parallel to y-axis.
    end
    
    methods (Access = public)
        function update(obj, rd)
            % Incorporates RobotData 'rd' into new map approximation.
            
            % Add new points to walls
            alignment = rd.getAlignment();
            switch alignment
                case {'+x', '-x'}
                    if rd.sFvalid, obj.addToYWalls(rd.sonarF); end
                    if rd.sBvalid, obj.addToYWalls(rd.sonarB); end
                    if rd.sLvalid, obj.addToXWalls(rd.sonarL); end
                    if rd.sRvalid, obj.addToXWalls(rd.sonarR); end
                case {'+y', '-y'}
                    if rd.sFvalid, obj.addToXWalls(rd.sonarF); end
                    if rd.sBvalid, obj.addToXWalls(rd.sonarB); end
                    if rd.sLvalid, obj.addToYWalls(rd.sonarL); end
                    if rd.sRvalid, obj.addToYWalls(rd.sonarR); end
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
            % Plots x-walls in blue and y-walls in green.
            for i = 1:length(obj.xWalls)
                obj.xWalls(i).plot('-b');
            end
            for i = 1:length(obj.yWalls)
                obj.yWalls(i).plot('-g');
            end
        end
    end
    methods (Access = private)
        function addToXWalls(obj, point)
            % Adds point to one of the x-walls or creates a new one if
            % point doesn't fit in any existing x-walls.
            for i = 1:length(obj.xWalls)
                if obj.xWalls(i).fitsPoint(point)
                    obj.xWalls(i).addPoint(point);
                    return
                end
            end
            obj.xWalls(end+1) = SonarWallX(point);
        end
        function addToYWalls(obj, point)
            % Adds point to one of the y-walls or creates a new one if
            % point doesn't fit in any existing y-walls.
            for i = 1:length(obj.yWalls)
                if obj.yWalls(i).fitsPoint(point)
                    obj.yWalls(i).addPoint(point);
                    return
                end
            end
            obj.yWalls(end+1) = SonarWallY(point);
        end
    end
end