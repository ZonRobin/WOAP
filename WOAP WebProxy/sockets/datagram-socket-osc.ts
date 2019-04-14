import * as dgram from "dgram";
import SocketsDatagramSocket from "./datagram-socket";
import OSCMessage from "../osc/osc-message";
import OSCBundle from "../osc/osc-bundle";
import OSCMapper from "../osc/osc-mapper";
import {OSCListener} from "../osc/osc-listener";

export default abstract class SocketsDatagramSocketOSC extends SocketsDatagramSocket implements OSCListener {
    /**
     * Triggered when OSC message was received
     *
     * @param {OSCMessage} message
     */
    public abstract oscMessageReceived(message: OSCMessage);

    /**
     * Triggered when OSC bundle was received
     *
     * @param {OSCBundle} bundle
     */
    public abstract oscBundleReceived(bundle: OSCBundle);

    /**
     * Callback triggered when message is received
     *
     * @param {Buffer} message
     * @param {dgram.RemoteInfo} info
     */
    protected messageReceivedCallback(message: Buffer, info: dgram.RemoteInfo): void {
        if (message.length === 0) {
            return;
        }

        let oscElement = OSCMapper.fromBuffer(message);
        if (oscElement.isMessage()) {
            this.oscMessageReceived(oscElement as OSCMessage);
        }
        else {
            this.oscBundleReceived(oscElement as OSCBundle);
        }
    }

    /**
     * Encodes and sends OSC message
     *
     * @param {OSCMessage} message
     */
    public sendOSCMessage(message: OSCMessage): void {
        this.sendMessage(message.toBuffer());
    }

    /**
     * Encodes and sends OSC bundle
     *
     * @param {OSCBundle} bundle
     */
    public sendOSCBundle(bundle: OSCBundle): void {
        this.sendMessage(bundle.toBuffer());
    }
}
