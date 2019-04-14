import ValidatableOSCMessage from "./messages/validatable-osc-message";
import {AddressWithTypeId, MessageReceiver} from "./api-core";
import OSCMessage from "../osc/osc-message";
import OSCBundle from "../osc/osc-bundle";
import ValidatableOSCMessageWithId from "./messages/validatable-osc-message-with-id";

export abstract class APIConsumer implements MessageReceiver {
    public initializeAPIConsumer(): void {
        window.WOAP.ServiceLocator.getAPIWorker().addListenerBatch(this, this.getMessageTypesToListenTo());
        this.sendAPIMessages(this.getMessagesToSendOnInit());
    }

    public terminateAPIConsumer(): void {
        window.WOAP.ServiceLocator.getAPIWorker().removeListenerBatch(this, this.getMessageTypesToListenTo());
        this.sendAPIMessages(this.getMessagesToSendOnTerminate());
    }

    public sendAPIMessage(message: ValidatableOSCMessage): void {
        window.WOAP.ServiceLocator.getAPIWorker().sendMessage(message);
    }

    public sendAPIMessages(messages: ValidatableOSCMessage[]): void {
        let worker = window.WOAP.ServiceLocator.getAPIWorker();
        for (let message of messages) {
            worker.sendMessage(message);
        }
    }

    public request(message: ValidatableOSCMessageWithId, responseAddress: string): Promise<OSCMessage> {
        return window.WOAP.ServiceLocator.getAPIWorker().request(message, responseAddress);
    }

    public getMessageTypesToListenTo(): AddressWithTypeId[] {
        return [];
    }

    public getMessagesToSendOnInit(): ValidatableOSCMessage[] {
        return [];
    }

    public getMessagesToSendOnTerminate(): ValidatableOSCMessage[] {
        return [];
    }

    public oscMessageReceived(message: OSCMessage, messageType: number): void {
    }

    public oscBundleReceived(bundle: OSCBundle, messageType: number): void {
    }
}