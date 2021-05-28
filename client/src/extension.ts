/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */
import * as path from 'path';
import {ExtensionContext, workspace} from 'vscode';

import {Executable, LanguageClient, LanguageClientOptions, ServerOptions} from 'vscode-languageclient/node';

let client: LanguageClient;

export function activate(context: ExtensionContext) {

	// let serverStartupCommand = context.asAbsolutePath(
    //     path.join('server', 'out', 'gw_basic_server')
    // );
	let serverStartupCommand = path.join(__dirname, '..', '..', 'server', 'out', 'gw_basic_server');
	console.log(serverStartupCommand);
	let serverExe: Executable = {
		command: serverStartupCommand,	
		args: ['config_file=' + path.join(__dirname, '..', '..', 'data', 'config.json'),
				'log_file='+path.join(__dirname, '..', '..', 'log.txt')],
		options: {
			env: process.env,
			shell: true
		}
	}

	let serverOptions: ServerOptions = {
		run:  serverExe,
		debug: serverExe
	}

	let clientOptions: LanguageClientOptions = {
		documentSelector: [{ scheme: 'file', pattern: '**'}],
		synchronize: {
			fileEvents: workspace.createFileSystemWatcher('**/.clientrc')
		}
	};

	client = new LanguageClient(
		'gw_basic_server',
		'gw_basic_server',
		serverOptions,
		clientOptions
	);

	client.start();
}

