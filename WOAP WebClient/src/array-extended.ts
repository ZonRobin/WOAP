import {Comparable} from "./comparable";

export class ArrayExtended<ItemType> {
    /**
     * @type {Array}
     */
    protected elements: Array<ItemType> = [];

    /**
     * @param {Array} elements
     */
    constructor(elements: Array<ItemType> = []) {
        this.elements = elements;
    }

    /**
     * @returns {Array}
     */
    public toArray(): Array<ItemType> {
        return this.elements;
    }

    /**
     * @returns {number}
     */
    public getLength(): number {
        return this.elements.length;
    }

    /**
     * @returns {boolean}
     */
    public isEmpty(): boolean {
        return this.elements.length === 0;
    }

    /**
     * @returns {boolean}
     */
    public isNotEmpty(): boolean {
        return this.elements.length !== 0;
    }

    /**
     * @param itemToRemove
     * @return {boolean} True if removed
     */
    public remove(itemToRemove: ItemType): boolean {
        let index = this.elements.indexOf(itemToRemove);
        if (index > -1) {
            this.elements.splice(index, 1);
            return true;
        }

        return false;
    }

    /**
     * @return {void}
     */
    public clear(): this {
        this.elements = [];
        return this;
    }

    /**
     * @param {number} index
     * @returns {ArrayExtended}
     */
    public removeAtIndex(index: number): this {
        if (index < 0 || index >= this.elements.length)
            return this;

        this.elements.splice(index, 1);
        return this;
    }

    /**
     * @param itemToSearch
     * @returns {boolean}
     */
    public has(itemToSearch: ItemType): boolean {
        return this.indexOf(itemToSearch) !== -1;
    }

    /**
     * @param {(item: ItemType) => number} callback
     * @return {any}
     */
    public findClosest(callback: (item: ItemType) => number): ItemType {
        if (this.elements.length === 0) {
            return null;
        }

        let closestDelta = Number.MAX_VALUE;
        let closestValue = null;

        for (let element of this.elements) {
            let delta = Math.abs(callback(element));
            if (delta < closestDelta) {
                closestDelta = delta;
                closestValue = element;
            }
        }

        return closestValue;
    }

    /**
     * @param {(item: ItemType) => number} callback
     * @return {number}
     */
    public findIndexOfClosest(callback: (item: ItemType) => number): number {
        if (this.elements.length === 0) {
            return null;
        }

        let closestDelta = Number.MAX_VALUE;
        let closestIndex = null;

        for (let i = 0; i < this.elements.length; ++i) {
            let delta = Math.abs(callback(this.elements[i]));
            if (delta < closestDelta) {
                closestDelta = delta;
                closestIndex = i;
            }
        }

        return closestIndex;
    }


    /**
     * @param {(item: ItemType) => boolean} callback
     * @return {any}
     */
    public find(callback: (item: ItemType) => boolean): ItemType {
        for (let element of this.elements) {
            if (callback(element)) {
                return element;
            }
        }

        return null;
    }

    /**
     * @param {(item: ItemType) => boolean} callback
     * @return {number}
     */
    public findIndex(callback: (item: ItemType) => boolean): number {
        let index = 0;
        for (let element of this.elements) {
            if (callback(element)) {
                return index;
            }
            ++index;
        }

        return -1;
    }

    /**
     * @param {(item: ItemType) => boolean} callback
     * @return {boolean}
     */
    public findAndRemove(callback: (item: ItemType) => boolean): boolean {
        for (let i = 0, len = this.elements.length; i < len; ++i) {
            if (callback(this.elements[i])) {
                this.removeAtIndex(i);
                return true;
            }
        }

        return false;
    }

    /**
     * @param {number} index
     * @returns {any}
     */
    public getUnchecked(index: number): ItemType {
        return this.elements[index];
    }

    /**
     * Returns element at index or null
     *
     * @param {number} index
     * @param {any} defaultValue
     * @returns {any|null}
     */
    public get(index: number, defaultValue: ItemType = null): ItemType {
        if (index < 0 || index >= this.elements.length) {
            return defaultValue;
        }

        return this.elements[index];
    }

    /**
     * @returns {any}
     */
    public getFirst(): ItemType {
        if (this.elements.length === 0) {
            return null;
        }

        return this.elements[0];
    }

    /**
     * @returns {any}
     */
    public getLast(): ItemType {
        if (this.elements.length === 0) {
            return null;
        }

        return this.elements[this.elements.length - 1];
    }

    /**
     * @param itemToAdd
     * @returns {ArrayExtended}
     */
    public add(itemToAdd: ItemType): this {
        this.elements.push(itemToAdd);

        return this;
    }

    /**
     * @param position
     * @param itemToAdd
     * @returns {ArrayExtended}
     */
    public insert(position: number, itemToAdd: ItemType): this {
        if (position < 0 || position >= this.elements.length)
            this.elements.push(itemToAdd);
        else
            this.elements.splice(position, 0, itemToAdd);

        return this;
    }

    /**
     * @param itemToAdd
     * @returns {ArrayExtended}
     */
    public addUnique(itemToAdd: ItemType): this {
        if (this.indexOf(itemToAdd) === -1) {
            this.elements.push(itemToAdd);
        }

        return this;
    }

    /**
     * @returns {any}
     */
    public pop(): ItemType {
        if (this.elements.length === 0) {
            return null;
        }

        let index = this.elements.length - 1;
        let element = this.elements[index];
        this.removeAtIndex(index);
        return element;
    }

    /**
     * @param {array} itemArray
     */
    public setArray(itemArray: ItemType[]): this {
        this.elements = itemArray;
        return this;
    }

    /**
     * @param {(item: ItemType) => void} callback
     * @return {ArrayExtended}
     */
    public forEach(callback: (item: ItemType) => void): this {
        for (let element of this.elements) {
            callback(element);
        }

        return this;
    }

    /**
     * @param {any[]} array
     * @param {(item: void) => void} callback
     * @returns {void}
     */
    public static forEach(array: any[], callback: (item: any) => void): void {
        for (let element of array) {
            callback(element);
        }
    }

    /**
     * @param {any[]} array
     * @param {(index: number, item: void) => void} callback
     * @returns {void}
     */
    public static forEachIndexed(array: any[], callback: (index: number, item: any) => void): void {
        let index = 0;
        for (let element of array) {
            callback(index++, element);
        }
    }

    /**
     * @param itemToSearch
     * @returns {number}
     */
    public indexOf(itemToSearch: ItemType): number {
        for (let i = 0, len = this.elements.length; i < len; ++i) {
            if (this.elements[i] === itemToSearch) {
                return i;
            }
        }

        return -1;
    }

    /**
     * @param value
     * @param {number} newIndex
     * @returns {ArrayExtended}
     */
    public move(value: ItemType, newIndex: number): this {
        return this.moveAtIndex(this.indexOf(value), newIndex);
    }

    /**
     * @param {number} itemIndex
     * @param {number} newIndex
     * @returns {ArrayExtended}
     */
    public moveAtIndex(itemIndex: number, newIndex: number): this {
        if (itemIndex < 0 || itemIndex >= this.elements.length || itemIndex === newIndex) {
            return this;
        }

        if (newIndex < 0 || newIndex >= this.elements.length) {
            newIndex = this.elements.length - 1;
        }

        this.elements.splice(newIndex, 0, this.elements.splice(itemIndex, 1)[0]);
        return this;
    }

    /**
     * @param {(value: ItemType, index: number, array: ItemType[]) => U} callback
     * @param {any} thisArg
     * @returns {U[]}
     */
    public map<U>(callback: (value: ItemType, index: number, array: ItemType[]) => U, thisArg?: any): U[] {
        return this.elements.map(callback, thisArg);
    }
}

export class ObjectArray<ItemType extends Comparable> extends ArrayExtended<ItemType> {
    /**
     * @param itemToRemove
     * @returns {boolean} True if removed
     */
    public remove(itemToRemove: ItemType): boolean {
        for (let i = 0, len = this.elements.length; i < len; ++i) {
            if (this.elements[i].equals(itemToRemove)) {
                this.elements.splice(i, 1);
                return true;
            }
        }

        return false;
    }

    /**
     * @param itemToSearch
     * @returns {number}
     */
    public indexOf(itemToSearch: ItemType): number {
        for (let i = 0, len = this.elements.length; i < len; ++i) {
            if (this.elements[i].equals(itemToSearch)) {
                return i;
            }
        }

        return -1;
    }
}