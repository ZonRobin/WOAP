import {ArrayExtended} from "./array-extended";
import * as Cookies from "es-cookie";

export type UUID = string;

/* To import:
 import {Utility} from "./utility";
 */

export module Utility {
    export const LeftMouseButton = 0;
    export const EnterKey = 13;
    export const EscapeKey = 27;

    export class Position {
        x: number;
        y: number;

        constructor(x: number = 0, y: number = 0) {
            this.x = x;
            this.y = y;
        }

        /**
         * Returns absolute distance between two points
         *
         * @param {Position} other
         * @returns {number}
         */
        public getAbsoluteDistance(other: Position): number {
            return Math.abs(this.x - other.x) + Math.abs(this.y - other.y);
        }

        /**
         * Returns distance with direction
         * @param {Position} other
         * @returns {number}
         */
        public getDirectionalDistance(other: Position): number {
            return (this.x - other.x) + (this.y - other.y);
        }
    }

    export interface ResizeWatcherListener {
        documentResized(event, orientationChanged: boolean);
    }

    export class ResizeWatcher {
        protected boundResizeCallback;
        protected boundOrientationCallback;
        protected listeners: ArrayExtended<ResizeWatcherListener> = new ArrayExtended();
        protected resizeThrottleTimeout = null;

        constructor() {
            this.boundResizeCallback = this.resizeCallback.bind(this);
            this.boundOrientationCallback = this.orientationCallback.bind(this);
            Utility.addEventListener(window, "resize", this.boundResizeCallback);
            Utility.addEventListener(window, "orientationchange", this.boundOrientationCallback);
        }

        public stop() {
            Utility.removeEventListener(window, "resize", this.boundResizeCallback);
            Utility.removeEventListener(window, "orientationchange", this.boundOrientationCallback);
        }

        public resizeCallback(event) {
            if (this.resizeThrottleTimeout !== null)
                return;

            this.resizeThrottleTimeout = setTimeout(() => {
                this.listeners.forEach((listener: ResizeWatcherListener) => listener.documentResized(event, false));
                this.resizeThrottleTimeout = null;
            }, 300);
        }

        public orientationCallback(event) {
            this.listeners.forEach((listener: ResizeWatcherListener) => listener.documentResized(event, true));
        }

        public addListener(listener: ResizeWatcherListener): void {
            this.listeners.addUnique(listener);
        }

        public removeListener(listener: ResizeWatcherListener): void {
            this.listeners.remove(listener);
        }
    }

    export interface MouseWatcherListener {
        dragStarted(position: Position): void;

        dragMove(position: Position): void;

        dragEnded(position: Position): void;
    }

    export class MouseWatcher {
        protected boundDragStartCallback;
        protected boundDragCallback;
        protected boundDragEndCallback;
        protected lastTouchEndTime: number = 0;
        protected dragStartMousePosition: Position = new Position();
        protected mouseDown: boolean = false;
        protected mouseDraggedSinceMouseDown: boolean = false;
        protected listeners: ArrayExtended<MouseWatcherListener> = new ArrayExtended();
        protected lastTouchMovePosition: Position = new Position();

        constructor() {
            this.boundDragStartCallback = this.dragStartCallback.bind(this);
            this.boundDragCallback = this.dragCallback.bind(this);
            this.boundDragEndCallback = this.dragEndCallback.bind(this);

            Utility.addEventListener(window, "mousedown", this.boundDragStartCallback);
            Utility.addEventListener(window, "touchstart", this.boundDragStartCallback);
            Utility.addEventListener(window, "mouseup", this.boundDragEndCallback);
            Utility.addEventListener(window, "touchend", this.boundDragEndCallback);
            Utility.addEventListener(window, "touchcancel", this.boundDragEndCallback);
        }

        public stop() {
            Utility.removeEventListener(window, "mousedown", this.boundDragStartCallback);
            Utility.removeEventListener(window, "touchstart", this.boundDragStartCallback);
            Utility.removeEventListener(window, "mouseup", this.boundDragEndCallback);
            Utility.removeEventListener(window, "touchend", this.boundDragEndCallback);
            Utility.removeEventListener(window, "touchcancel", this.boundDragEndCallback);
        }

        public isMouseDraggedSinceMouseDown(): boolean {
            return this.mouseDraggedSinceMouseDown;
        }

        public isMouseDown(): boolean {
            return this.mouseDown;
        }

        public addListener(listener: MouseWatcherListener) {
            this.listeners.add(listener);
        }

        public removeListener(listener: MouseWatcherListener) {
            this.listeners.remove(listener);
        }

        public wasTouchEndLately(): boolean {
            return (new Date().getTime()) - this.lastTouchEndTime < 1500;
        }

        protected dragStartCallback(event) {
            this.dragStartMousePosition = Utility.getTouchPosition(event);
            this.mouseDraggedSinceMouseDown = false;
            this.mouseDown = true;
            Utility.addEventListener(window, "mousemove", this.boundDragCallback);
            Utility.addEventListener(window, "touchmove", this.boundDragCallback);
            this.listeners.forEach((listener) => listener.dragStarted(this.dragStartMousePosition));
        }

        protected dragCallback(event) {
            this.lastTouchMovePosition = Utility.getTouchPosition(event);

            if (!this.mouseDraggedSinceMouseDown && this.lastTouchMovePosition.getAbsoluteDistance(this.dragStartMousePosition) > 5) {
                this.mouseDraggedSinceMouseDown = true;
            }

            this.listeners.forEach((listener) => listener.dragMove(this.lastTouchMovePosition));
        }

        protected dragEndCallback(ev) {
            if (ev.type === "touchend") {
                this.lastTouchEndTime = new Date().getTime();
            }

            this.mouseDown = false;
            Utility.removeEventListener(window, "mousemove", this.boundDragCallback);
            Utility.removeEventListener(window, "touchmove", this.boundDragCallback);
            this.listeners.forEach((listener) => listener.dragEnded(this.lastTouchMovePosition));
        }
    }

    export class ContextMenuManager {
        public menuClosedTime: number = -1;

        public contextMenuWasOpened(): void {
        }

        public contextMenuWasClosed(): void {
            this.menuClosedTime = new Date().getTime();
        }

        public isContextMenuOpened(): boolean {
            return new Date().getTime() - this.menuClosedTime < 300;
        }

        public dismissMenu(): void {
            Utility.triggerEvent(document.children[0], "click");
        }
    }

    /**
     * @param {number} first
     * @param {number} second
     * @returns {number}
     */
    export function higherOf(first: number, second: number): number {
        if (first > second)
            return first;
        else
            return second;
    }

    /**
     * @param {number} first
     * @param {number} second
     * @returns {number}
     */
    export function lowerOf(first: number, second: number): number {
        if (first < second)
            return first;
        else
            return second;
    }

    /**
     * @param {number} min
     * @param {number} max
     * @param {number} value
     * @returns {number}
     */
    export function inRange(min: number, max: number, value: number): number {
        if (value <= min)
            return min;

        if (value >= max)
            return max;

        return value;
    }

    /**
     * @param {number} min
     * @param {number} max
     * @param {number} value
     * @returns {boolean}
     */
    export function isInRange(min: number, max: number, value: number): boolean {
        return !(value < min || value > max);
    }

    /**
     * @param {number} value
     * @param {number} numDecimals
     * @returns {number}
     */
    export function round(value: number, numDecimals: number = 0): number {
        let powered = Math.pow(10, numDecimals);
        return Math.floor(value * powered) / powered;
    }

    /**
     * @param {string} str
     * @returns {string}
     */
    export function trimString(str: string): string {
        str = str.replace(/^\s\s*/, "");
        let ws = /\s/;
        let i = str.length;

        while (ws.test(str.charAt(--i))) {
        }

        return str.slice(0, i + 1);
    }

    /**
     * @param {string} script
     * @param {string} elementId
     */
    export function domAddScript(script: string, elementId: string = "") {
        const tag = document.createElement("script");
        tag.type = "text/javascript";
        tag.innerHTML = "{\n" + script + "\n}";

        if (elementId.length > 0) {
            tag.id = elementId;
        }

        document.body.appendChild(tag);
    }

    /**
     * @param {string} script
     * @param {string} closureId
     */
    export function domAddScriptInClosure(script: string, closureId: string) {
        const tag = document.createElement("script");
        tag.type = "text/javascript";
        tag.innerHTML =
            "(function() {\n"
            + script
            + "\n})();";
        tag.id = closureId;

        document.body.appendChild(tag);
    }

    /**
     * @param {string} elementId
     */
    export function domRemoveElement(elementId: string) {
        let element = document.getElementById(elementId);
        if (element === null) {
            return;
        }
        element.parentNode.removeChild(element);
    }

    /**
     * @param {string} xmlString
     * @returns {Document}
     * @throws {Error}
     */
    export function parseXML(xmlString: string): Document {
        let xml;

        if (!xmlString || typeof xmlString !== "string") {
            throw new Error("Invalid XML: " + xmlString);
        }

        try {
            xml = (new DOMParser()).parseFromString(xmlString, "text/xml");
        }
        catch (error) {
            throw new Error("Invalid XML: " + xmlString);
        }

        if (xml.getElementsByTagNameNS("*", "parsererror").length > 0) {
            throw new Error("Invalid XML: " + xmlString);
        }

        return xml;
    }

    /**
     * @param {boolean} boolValue
     * @returns {number}
     */
    export function boolToInt(boolValue: boolean): number {
        return boolValue ? 1 : 0;
    }

    /**
     * @param {number} intValue
     * @returns {boolean}
     */
    export function intToBool(intValue: number): boolean {
        return intValue > 0;
    }

    /**
     * @param {any} field
     * @returns {boolean}
     */
    export function isDefined(field: any): boolean {
        return typeof field !== "undefined";
    }

    /**
     * @param {any} field
     * @returns {boolean}
     */
    export function isUndefined(field: any): boolean {
        return typeof field === "undefined";
    }

    /**
     * @param {number} decibels
     * @returns {number}
     */
    export function decibelsToGain(decibels: number): number {
        return Math.pow(10, decibels / 20);
    }

    /**
     * @param {number} gain
     * @param {number} negativeInfinityDb
     * @returns {number}
     */
    export function gainToDecibels(gain: number, negativeInfinityDb: number = -145): number {
        if (gain <= 0)
            return negativeInfinityDb;

        return Utility.higherOf(negativeInfinityDb, 20 * Math.log10(gain));
    }

    /**
     * Returns default value if "value" is undefined
     *
     * @param {any} value
     * @param {any} defaultValue
     * @returns {any}
     */
    export function xDef(value: any, defaultValue: any): any {
        if (typeof value === "undefined") {
            return defaultValue;
        }

        return value;
    }

    /**
     * Returns default value if "value" is undefined or  null
     *
     * @param {any} value
     * @param {any} defaultValue
     * @returns {any}
     */
    export function xNull(value: any, defaultValue: any): any {
        if (typeof value === "undefined" || value === null) {
            return defaultValue;
        }

        return value;
    }

    /**
     * Returns default value if "value" is NaN
     *
     * @param {any} value
     * @param {any} defaultValue
     * @returns {any}
     */
    export function xNaN(value: any, defaultValue: any): any {
        if (isNaN(value)) {
            return defaultValue;
        }

        return value;
    }

    /**
     * @param {any} value
     * @returns {any}
     */
    export function NaNToUndef(value: any): any {
        if (isNaN(value)) {
            return undefined;
        } else {
            return value;
        }
    }

    /**
     * @param {string} str
     * @returns {string}
     */
    export function brToNl(str: string): string {
        return str.replace(/<br\s*\/?>/mg, "\n");
    }

    /**
     * @param {string} str
     * @returns {string}
     */
    export function nlToBr(str: string): string {
        return str.replace(/(?:\r\n|\r|\n)/g, "<br />");
    }

    /**
     * @param {string} str
     * @returns {boolean}
     */
    export function strToBool(str: string): boolean {
        return typeof str === "string" && str.length !== 0 && str !== "0";
    }

    /**
     * @param {string} str
     * @param {number} defaultValue
     * @returns {number}
     */
    export function strToInt(str: string, defaultValue: number): number {
        let value = parseInt(str, 10);
        if (isNaN(value)) {
            return defaultValue;
        } else {
            return value;
        }
    }

    /**
     * @param {string} str
     * @param {number} defaultValue
     * @returns {number}
     */
    export function strToFloat(str: string, defaultValue: number): number {
        let value = parseFloat(str);
        if (isNaN(value)) {
            return defaultValue;
        } else {
            return value;
        }
    }

    /**
     * @param {string} str
     * @returns {string}
     */
    export function toAlphanumeric(str: string): string {
        return str.replace(/\W/g, "");
    }

    /**
     * @param {number} min
     * @param {number} max
     * @returns {number}
     */
    export function rand(min: number, max: number): number {
        return Math.floor(Math.random() * (max - min) + min);
    }

    /**
     * @param {number} numDigits
     * @returns {number}
     */
    export function randDigits(numDigits: number): number {
        return Utility.rand(Math.pow(10, numDigits - 1), Math.pow(10, numDigits) - 1);
    }

    /**
     * @param {string} string
     * @returns {string}
     */
    export function capitalizeFirst(string: string): string {
        return string.charAt(0).toUpperCase() + string.slice(1);
    }

    /**
     * @param {Element} object
     * @param {string} type
     * @param {(event) => void} callback
     * @param {{}} args
     */
    export function addEventListener(object, type: string, callback: (event) => void, args: {} = {}) {
        if (object == null || typeof(object) === "undefined")
            return;

        if (object.addEventListener) {
            object.addEventListener(type, callback, false, args);
        } else if (object.attachEvent) {
            object.attachEvent("on" + type, callback);
        } else {
            object["on" + type] = callback;
        }
    }

    /**
     * @param {Element} object
     * @param {string} type
     * @param {(event) => void} callback
     */
    export function removeEventListener(object, type: string, callback: (event) => void) {
        if (object == null || typeof(object) === "undefined")
            return;

        if (object.removeEventListener) {
            object.removeEventListener(type, callback, false);
        } else if (object.detachEvent) {
            object.detachEvent("on" + type, callback);
        } else if (object["on" + type] === callback) {
            object["on" + type] = undefined;
        }
    }

    /**
     * Returns that crazy SVG path for rounded rectangles
     *
     * @param {number} x
     * @param {number} y
     * @param {number} width
     * @param {number} height
     * @param {number} cornerRadius
     * @param {boolean} topLeft
     * @param {boolean} topRight
     * @param {boolean} bottomLeft
     * @param {boolean} bottomRight
     * @returns {string}
     */
    export function roundedRectanglePath(x: number, y: number, width: number, height: number, cornerRadius: number, topLeft: boolean, topRight: boolean, bottomLeft: boolean, bottomRight: boolean): string {
        let val = "M" + (x + cornerRadius) + "," + y;

        val += " h" + (width - 2 * cornerRadius);
        if (topRight) {
            val += " a" + cornerRadius + "," + cornerRadius + " 0 0 1 " + cornerRadius + "," + cornerRadius;
        } else {
            val += " h" + cornerRadius;
            val += " v" + cornerRadius;
        }

        val += " v" + (height - 2 * cornerRadius);
        if (bottomRight) {
            val += " a" + cornerRadius + "," + cornerRadius + " 0 0 1 " + -cornerRadius + "," + cornerRadius;
        } else {
            val += " v" + cornerRadius;
            val += " h" + -cornerRadius;
        }

        val += " h" + (2 * cornerRadius - width);
        if (bottomLeft) {
            val += " a" + cornerRadius + "," + cornerRadius + " 0 0 1 " + -cornerRadius + "," + -cornerRadius;
        } else {
            val += " h" + -cornerRadius;
            val += " v" + -cornerRadius;
        }

        val += " v" + (2 * cornerRadius - height);
        if (topLeft) {
            val += " a" + cornerRadius + "," + cornerRadius + " 0 0 1 " + cornerRadius + "," + -cornerRadius;
        } else {
            val += " v" + -cornerRadius;
            val += " h" + cornerRadius;
        }

        val += " z";
        return val;
    }

    /**
     * Reads int16 sample from block, converting it to float. Unchecked.
     *
     * @param {DataView} data
     * @param {number} sampleIndex
     * @returns {number}
     */
    export function readInt16SampleFromBlock(data: DataView, sampleIndex: number): number {
        return data.getInt16(sampleIndex * 2, true) / 32768;
    }

    /**
     * Reads signal level sample (uint16 with max 24dBFs) from block, converting it to float. Unchecked.
     *
     * @param {DataView} data
     * @param {number} sampleIndex
     * @returns {number}
     */
    export function readSignalLevelSampleFromBlock(data: DataView, sampleIndex: number): number {
        return data.getUint16(sampleIndex * 2, true) / 8192;
    }

    /**
     * Reads bit value from block, converting it to bool. Unchecked
     *
     * @param {DataView} data
     * @param {number} bitIndex
     * @returns {boolean}
     */
    export function readBitFromBlock(data: DataView, bitIndex: number): boolean {
        return 0 !== (data.getUint8(bitIndex >> 3) & (1 << (bitIndex % 8)));
    }

    /**
     * @param event
     * @returns {Utility.Position}
     */
    export function getTouchPosition(event): Position {
        if (typeof event.targetTouches !== "undefined")
            return new Position(event.targetTouches[0].pageX, event.targetTouches[0].pageY);
        else
            return new Position(event.pageX, event.pageY);
    }

    /**
     * Returns horizontal and vertical scrollbar sizes
     * @returns {number}
     */
    export function getScrollbarsSize(): { horizontalBarHeight: number, verticalBarWidth: number } {
        let outer = document.createElement("div");
        outer.style.visibility = "hidden";
        outer.style.width = "100px";
        outer.style.height = "100px";
        outer.style.msOverflowStyle = "scrollbar";

        document.body.appendChild(outer);

        let widthNoScroll = outer.offsetWidth;
        let heightNoScroll = outer.offsetHeight;
        outer.style.overflow = "scroll";

        let inner = document.createElement("div");
        inner.style.width = "100%";
        inner.style.height = "100%";
        outer.appendChild(inner);

        let widthWithScroll = inner.offsetWidth;
        let heightWithScroll = inner.offsetHeight;
        outer.parentNode.removeChild(outer);

        return {
            horizontalBarHeight: heightNoScroll - heightWithScroll,
            verticalBarWidth: widthNoScroll - widthWithScroll
        };
    }

    /**
     * @param {Element} element
     * @param {string} eventType
     */
    export function triggerEvent(element: Element, eventType: string): void {
        if (element === null || typeof element === "undefined")
            return;

        if (typeof (element as any).fireEvent !== "undefined") {
            (element as any).fireEvent("on" + eventType);
        } else {
            let evObj = document.createEvent("Events");
            evObj.initEvent(eventType, true, false);
            element.dispatchEvent(evObj);
        }
    }

    /**
     * @param {string} name
     * @param {string} value
     */
    export function setCookie(name: string, value: string): void {
        Cookies.set(name, value, {expires: 365, path: "", domain: window.location.hostname});
    }

    /**
     * @param {string} name
     * @returns {string}
     */
    export function getCookie(name: string): string {
        return xDef(Cookies.get(name), "");
    }

    /**
     * @param {string} name
     */
    export function removeCookie(name: string): void {
        Cookies.remove(name);
    }

    /**
     * @param {string} name
     * @param {string} value
     */
    export function setLocalStorageValue(name: string, value: string): void {
        window.localStorage.setItem(name, value);
    }

    /**
     * @param {string} name
     * @param {string} value
     */
    export function setLocalStorageValueWithTimestamp(name: string, value: string): void {
        window.localStorage.setItem(name, value + "@@" + new Date().getTime());
    }

    /**
     * @param {string} name
     * @returns {string}
     */
    export function getLocalStorageValue(name: string): string {
        return Utility.xNull(window.localStorage.getItem(name), "");
    }

    /**
     * @param {string} name
     * @return {string}
     */
    export function getLocalStorageValueUpdatingTimestamp(name: string): string {
        let data = Utility.xNull(window.localStorage.getItem(name), "");

        if (data.length === 0)
            return data;

        let timestampDelimiterIndex = data.indexOf("@@");
        if (timestampDelimiterIndex !== -1) {
            let value = data.substr(0, timestampDelimiterIndex);
            Utility.setLocalStorageValueWithTimestamp(name, value);
            return value;
        } else {
            Utility.setLocalStorageValueWithTimestamp(name, data);
            return data;
        }
    }

    export function clearOldLocalStorageValuesWithTimestamp(): void {
        let currentTime = new Date().getTime();
        let maxAge = 1000 * 60 * 60 * 24 * 30; // one month

        let keysToRemove = [];

        for (let key in window.localStorage) {
            if (!window.localStorage.hasOwnProperty(key))
                continue;

            let data = Utility.xNull(window.localStorage.getItem(key), "");

            let timestampDelimiterIndex = data.indexOf("@@");
            if (timestampDelimiterIndex !== -1) {
                let timestamp = parseInt(data.substr(timestampDelimiterIndex + 2), 10);
                if (currentTime - timestamp > maxAge) {
                    keysToRemove.push(key);
                }
            }
        }

        for (let key of keysToRemove) {
            window.localStorage.removeItem(key);
        }
    }

    /**
     * @returns {[number , number]}
     */
    export function getWindowSize(): [number, number] {
        let root = document.querySelector(".appRoot");
        if (root === null) {
            return [0, 0];
        }

        return [root.clientWidth, root.clientHeight];
    }
}