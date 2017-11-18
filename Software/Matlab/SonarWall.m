classdef SonarWall < handle
    %SONARWALL Class for sonar wall hypotheses.
    %   Created by RBE-2002 B17 Team 10.
    %   
    %   This class represents hypotheses for walls based on sonar and
    %   odometry data from the firefighter robot. Walls can be created
    %   from a single point and expanded by adding nearby points.
    %   
    %   See also: SONARWALLX, SONARWALLY
    
    properties (Access = protected, Constant)
        edgeLimit = 0.07;
        normalLimit = 0.02;
    end
    
    properties (Access = public) % MAKE PROTECTED POST DEBUG
        age = 0;    % How many iterations old the wall is
        points = 1; % How many points the wall contains
    end
    
    methods
        function [age] = incrementAge(obj)
            % Adds 1 to age of wall and returns new age.
            obj.age = obj.age + 1;
            age = obj.age;
        end
        function [m] = isMistake(obj)
            % Returns logical 1 if wall is likely a sonar mistake
            m = ((obj.points < 2) && (obj.age > 1)) || ...
                ((obj.points < 4) && (obj.age > 5));
        end
    end
end