export namespace SocketsHelper {
    export interface DecoderListener {
        /**
         * @param {Buffer} message
         */
        messageReceived(message: Buffer): void;
    }

    /**
     * Adds prefix to buffer - this prefix contains int32 size of the message
     *
     * @param {Buffer} data
     * @returns {Buffer}
     */
    export function encodeBuffer(data: Buffer): Buffer {
        let messageHeader = new Buffer(4);
        messageHeader.writeInt32LE(data.byteLength, 0);

        return Buffer.concat([messageHeader, data]);
    }

    /**
     * Expects buffer to have the int32 lengths in prefix of each message
     * Decodes to array of messages
     *
     * @param {Buffer} data
     * @returns {Array<Buffer>}
     */
    export function decodeBuffer(data: Buffer): Array<Buffer> {
        let messages = [];

        let int32Length = 4;
        let dataLength = data.byteLength;

        for (let i = 0; i < dataLength;) {
            let messageLength = data.readInt32LE(i);
            let messageOffset = i + int32Length;

            messages.push(data.slice(messageOffset, messageOffset + messageLength));
            i += int32Length + messageLength;
        }

        return messages;
    }

    /**
     * Expects buffer to have the int32 lengths in prefix of each message
     * After each message it calls the callback instance
     *
     * @param {Buffer} data
     * @param {DecoderListener} listener
     * @returns {Array<Buffer>}
     */
    export function decodeBufferWithCallback(data: Buffer, listener: DecoderListener): void {
        let int32Length = 4;
        let dataLength = data.byteLength;

        for (let i = 0; i < dataLength;) {
            let messageLength = data.readInt32LE(i);
            let messageOffset = i + int32Length;

            listener.messageReceived(data.slice(messageOffset, messageOffset + messageLength));
            i += int32Length + messageLength;
        }
    }
}

