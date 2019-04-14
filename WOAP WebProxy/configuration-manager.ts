import Configuration from "./configuration";

export module ConfigurationManager {
    export function isDebug(): boolean {
        return Configuration.isDebug;
    }

    export function isDevelopment(): boolean {
        return Configuration.isDevelopment;
    }
}