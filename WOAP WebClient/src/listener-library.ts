import {Comparable} from "./comparable";
import {ObjectArray} from "./array-extended";
import {Utility} from "./utility";

export class ListenerLibraryItem<ListenerType extends Comparable> {
    public folders: Map<String, ListenerLibraryItem<ListenerType>> = new Map<String, ListenerLibraryItem<ListenerType>>();
    public listeners: ObjectArray<ListenerType> = new ObjectArray<ListenerType>();
}

export class ListenerLibrary<ListenerType extends Comparable> {
    protected root: ListenerLibraryItem<ListenerType> = new ListenerLibraryItem<ListenerType>();
    protected pathDelimiter: string;

    constructor(pathDelimiter: string = "/") {
        this.pathDelimiter = pathDelimiter;
    }

    public getListenersOrNull(path: string): ObjectArray<ListenerType> {
        let pathParts: Array<string> = path.split(this.pathDelimiter);
        let folder: ListenerLibraryItem<ListenerType> = this.root;

        for (let pathPart of pathParts) {
            let subFolder = folder.folders.get(pathPart);
            if (Utility.isUndefined(subFolder)) {
                return null;
            }

            folder = subFolder;
        }

        return folder.listeners;
    }

    public getListeners(path: string): ObjectArray<ListenerType> {
        let pathParts: Array<string> = path.split(this.pathDelimiter);
        let folder: ListenerLibraryItem<ListenerType> = this.root;

        for (let pathPart of pathParts) {
            let subFolder = folder.folders.get(pathPart);
            if (!Utility.isUndefined(subFolder)) {
                folder = subFolder;
            } else {
                let newItem: ListenerLibraryItem<ListenerType> = new ListenerLibraryItem<ListenerType>();
                folder.folders.set(pathPart, newItem);
                folder = newItem;
            }
        }

        return folder.listeners;
    }

    public addListener(path: string, listener: ListenerType): void {
        this.getListeners(path).add(listener);
    }

    public addListenerUnchecked(path: string, listener: ListenerType): void {
        let pathParts: Array<string> = path.split(this.pathDelimiter);
        let folder: ListenerLibraryItem<ListenerType> = this.root;

        for (let pathPart of pathParts) {
            folder = folder.folders.get(pathPart);
        }

        folder.listeners.add(listener);
    }

    public removeListener(path: string, listener: ListenerType): void {
        let listeners = this.getListenersOrNull(path);
        if (listeners !== null) {
            listeners.remove(listener);
        }
    }
}