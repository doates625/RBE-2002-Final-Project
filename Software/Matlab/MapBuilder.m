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
        
        refWallXp = SonarWallY([0; 0])  % Wall on +x side of robot.
        refWallXm = SonarWallY([0; 0])  % Wall on -x side of robot.
        refWallYp = SonarWallX([0; 0])  % Wall on +y side of robot.
        refWallYm = SonarWallX([0; 0])  % Wall on -y side of robot.
    end
    
    methods (Access = public)
        function [slip] = update(obj, rd)
            % Incorporates RobotData 'rd' into new map approximation and
            % returns estimated wheel slippage vector.
            % Perform: rd.pos = rd.pos - slip;
            slip = [0; 0];
            
            % Compute reference walls
            obj.refWallXp = obj.getRefWallXp(rd.pos);
            obj.refWallXm = obj.getRefWallXm(rd.pos);
            obj.refWallYp = obj.getRefWallYp(rd.pos);
            obj.refWallYm = obj.getRefWallYm(rd.pos);
            
            % Incorporate new points in map
            alignment = rd.getAlignment();
            switch alignment
                case {'+x', '-x'}
                    
                    % Calculate wheel slippage
                    %{
                    switch alignment
                        case '+x'
                            wallF = obj.refWallXp;   
                            wallB = obj.refWallXm;
                            wallL = obj.refWallYp;
                            wallR = obj.refWallYm;
                        case '-x'
                            wallF = obj.refWallXm;
                            wallB = obj.refWallXp;
                            wallL = obj.refWallYm;
                            wallR = obj.refWallYp;
                    end
                    if ~isempty(wallF)
                        slip = slip + [rd.sonarF(1) - wallF.xPos; 0];
                    end
                    if ~isempty(wallB)
                        slip = slip + [rd.sonarB(1) - wallB.xPos; 0];
                    end
                    if ~isempty(wallL)
                        slip = slip + [0; rd.sonarL(2) - wallL.yPos];
                    end
                    if ~isempty(wallR)
                        slip = slip + [0; rd.sonarR(2) - wallR.yPos];
                    end
                    if ~isempty(wallF) && ~isempty(wallB)
                        slip(1) = slip(1) * 0.5;
                    end
                    if ~isempty(wallL) && ~isempty(wallR)
                        slip(2) = slip(2) * 0.5;
                    end
                    %}

                    % Build y-walls from front and rear sonar
                    if rd.sFvalid, obj.addToYWalls(rd.sonarF); end
                    if rd.sBvalid, obj.addToYWalls(rd.sonarB); end

                    % Build x-walls from left and right sonar
                    if rd.sLvalid, obj.addToXWalls(rd.sonarL); end
                    if rd.sRvalid, obj.addToXWalls(rd.sonarR); end                 
                case {'+y', '-y'}
                    
                    % Calculate wheel slippage
                    %{
                    switch alignment
                        case '+y'
                            wallF = obj.refWallYp;
                            wallB = obj.refWallYm;
                            wallL = obj.refWallXm;
                            wallR = obj.refWallXp;
                        case '-y'
                            wallF = obj.refWallYm;
                            wallB = obj.refWallYp;
                            wallL = obj.refWallXp;
                            wallR = obj.refWallXm;
                    end
                    if ~isempty(wallF)
                        slip = slip + [0; rd.sonarF(2) - wallF.yPos];
                    end
                    if ~isempty(wallB)
                        slip = slip + [0; rd.sonarB(2) - wallB.yPos];
                    end
                    if ~isempty(wallL)
                        slip = slip + [rd.sonarL(1) - wallL.xPos; 0];
                    end
                    if ~isempty(wallR)
                        slip = slip + [rd.sonarR(1) - wallR.xPos; 0];
                    end
                    if ~isempty(wallF) && ~isempty(wallB)
                        slip(2) = slip(2) * 0.5;
                    end
                    if ~isempty(wallL) && ~isempty(wallR)
                        slip(1) = slip(1) * 0.5;
                    end
                    %}

                    % Build x-walls from front and rear sonar
                    if rd.sFvalid, obj.addToXWalls(rd.sonarF); end
                    if rd.sBvalid, obj.addToXWalls(rd.sonarB); end

                    % Build y-walls from left and right sonar
                    if rd.sLvalid, obj.addToYWalls(rd.sonarL); end
                    if rd.sRvalid, obj.addToYWalls(rd.sonarR); end
            end
            
            % Remove wheel slippage if not too large
            %{
            if norm(slip) <= 0.1
                rd.removeSlip(slip);
            else
                slip = [0; 0];
            end
            %}
            
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
                wall = obj.xWalls(i);
                if obj.isRefWallYp(wall) || obj.isRefWallYm(wall)
                    wall.plot('-go');
                else
                    wall.plot('-bo');
                end
            end
            for i = 1:length(obj.yWalls)
                wall = obj.yWalls(i);
                if obj.isRefWallXp(wall) || obj.isRefWallXm(wall)
                    wall.plot('-go');
                else
                    wall.plot('-ro');
                end
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
        function [wall] = getRefWallXp(obj, point)
            % Returns closest y-wall aligned to positive-x side of point.
            wall = SonarWallY.empty;
            for i = 1:length(obj.yWalls)
                if obj.yWalls(i).onNegativeSide(point)
                    if isempty(wall) || obj.yWalls(i).xPos < wall.xPos
                        wall = obj.yWalls(i);
                    end
                end
            end
        end
        function [wall] = getRefWallXm(obj, point)
            % Returns closest y-wall aligned to negative-x side of point.
            wall = SonarWallY.empty;
            for i = 1:length(obj.yWalls)
                if obj.yWalls(i).onPositiveSide(point)
                    if isempty(wall) || obj.yWalls(i).xPos > wall.xPos
                        wall = obj.yWalls(i);
                    end
                end
            end
        end
        function [wall] = getRefWallYp(obj, point)
            % Returns closest x-wall aligned to positive-y side of point.
            wall = SonarWallX.empty;
            for i = 1:length(obj.xWalls)
                if obj.xWalls(i).onNegativeSide(point)
                    if isempty(wall) || obj.xWalls(i).yPos < wall.yPos
                        wall = obj.xWalls(i);
                    end
                end
            end
        end
        function [wall] = getRefWallYm(obj, point)
            % Returns closest x-wall aligned to negative-y side of point.
            wall = SonarWallX.empty;
            for i = 1:length(obj.xWalls)
                if obj.xWalls(i).onPositiveSide(point)
                    if isempty(wall) || obj.xWalls(i).yPos > wall.yPos
                        wall = obj.xWalls(i);
                    end
                end
            end
        end
        function [f] = isRefWallXp(obj, wall)
            % Returns 1 if wall is the +x reference wall.
            f = ~isempty(obj.refWallXp) && eq(wall, obj.refWallXp);
        end
        function [f] = isRefWallXm(obj, wall)
            % Returns 1 if wall is the -x reference wall.
            f = ~isempty(obj.refWallXm) && eq(wall, obj.refWallXm);
        end
        function [f] = isRefWallYp(obj, wall)
            % Returns 1 if wall is the +y reference wall.
            f = ~isempty(obj.refWallYp) && eq(wall, obj.refWallYp);
        end
        function [f] = isRefWallYm(obj, wall)
            % Returns 1 if wall is the -y reference wall.
            f = ~isempty(obj.refWallYm) && eq(wall, obj.refWallYm);
        end
    end
end