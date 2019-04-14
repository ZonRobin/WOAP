import * as React from "react";
import App from "./app";
import {BrowserRouter, Route, Switch} from "react-router-dom";

interface AppContainerProps {

}

interface AppContainerState {

}

export default class AppContainer extends React.Component<AppContainerProps, AppContainerState> {
    public shouldComponentUpdate() {
        return false;
    }

    public render() {
        return (
            <BrowserRouter>
                <Switch>
                    <Route component={App}/>
                </Switch>
            </BrowserRouter>
        );
    }
}

