import Configuration from "./configuration";

export module ConfigurationManager {
    export function isDebug(): boolean {
        return Configuration.isDebug;
    }

    export function isDevelopment(): boolean {
        return Configuration.isDevelopment;
    }

    export function isLogAPICallsToConsole(): boolean {
        return Configuration.logAPICallsToConsole;
    }

    export function getAPIRequestTimeout(): number {
        return Configuration.apiRequestTimeout;
    }

    export function getSignalLevelsRefreshRate(): number {
        return Configuration.signalLevelsRefreshRate;
    }
}