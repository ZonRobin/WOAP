import App from "./app";
import StoresManager from "./stores/store-manager";
import PAGManager from "./pag/pag-manager";
import APICore from "./api/api-core";
import ModalManager from "./components/modal-manager";

export default class ServiceLocator {
    protected apiWorker: APICore;
    protected pagManager: PAGManager;
    protected mainComponent: App;
    protected modalManager: ModalManager;
    protected storesManager: StoresManager;

    public getMainComponent(): App {
        return this.mainComponent;
    }

    public getModalManager(): ModalManager {
        return this.modalManager;
    }

    public getAPIWorker(): APICore {
        return this.apiWorker;
    }

    public getPAGManager(): PAGManager {
        return this.pagManager;
    }

    public getStoresManager(): StoresManager {
        return this.storesManager;
    }

    public setMainComponent(main: App): this {
        this.mainComponent = main;
        return this;
    }

    public setModalManager(manager: ModalManager): this {
        this.modalManager = manager;
        return this;
    }

    public setAPIWorker(worker: APICore): this {
        this.apiWorker = worker;
        return this;
    }

    public setPAGManager(manager: PAGManager): this {
        this.pagManager = manager;
        return this;
    }

    public setStoresManager(manager: StoresManager): this {
        this.storesManager = manager;
        return this;
    }
}