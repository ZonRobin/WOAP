import OSCMessage from './osc-message';
import OSCBundle from './osc-bundle';

export interface OSCListener {
    /**
     * @param {OSCMessage} message
     */
    oscMessageReceived(message: OSCMessage): void;

    /**
     * @param {OSCBundle} bundle
     */
    oscBundleReceived(bundle: OSCBundle): void;
}