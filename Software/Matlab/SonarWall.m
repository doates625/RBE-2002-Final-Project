classdef SonarWall < handle
    %SONARWALL Class for sonar wall hypotheses.
    %   Created by Dan Oates (RBE-2002 B17 Team 10).
    %   
    %   This class represents hypotheses for walls based on sonar and
    %   odometry data from the firefighter robot. Walls can be created
    %   from a single point and expanded by adding nearby points. All
    %   distances are in meters.
    %   
    %   See also: SONARWALLX, SONARWALLY
    
    properties (Access = public)
        age = 0;        % How many iterations old the wall is
        dormancy = 0;   % Number of iterations since last point added
        points = 1;     % How many points the wall contains
    end
    
    properties (Access = protected, Constant)
        radius = 0.01;      % Half of wall thickness (m).
        edgeLimit = 0.2;    % Maximum parallel edge offset of new point.
        normalLimit = 0.04; % Maximum perpendicular offset of new point.
    end
    
    methods
        function incrementAge(obj)
            % Adds 1 to age of wall and dormancy.
            obj.age = obj.age + 1;
            obj.dormancy = obj.dormancy + 1;
        end
        function [m] = isMistake(obj)
            % Returns logical 1 if wall is likely a sonar mistake
            m = ((obj.points < 2) && (obj.age > 1)) || ...
                ((obj.points < 4) && (obj.age > 5)) || ...
                ((obj.dormancy > 5) && (obj.getLength < 0.18));
        end
    end
end