/* --------------------------------------------------------------------------------------------
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License. See License.txt in the project root for license information.
 * ------------------------------------------------------------------------------------------ */

import {ExtensionContext, workspace} from 'vscode';

import {Executable, LanguageClient, LanguageClientOptions, ServerOptions} from 'vscode-languageclient/node';

let client: LanguageClient;

export function activate(context: ExtensionContext) {

	let serverStartupCommand = '/home/danil/CLionProjects/gw_basic_server/server/out/gw_basic_server';
	let serverExe: Executable = {
		command: serverStartupCommand,
		args: ['-lsp'],
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
		// Register the server for plain text documents
		documentSelector: [{ scheme: 'file',pattern: '*'}],
		synchronize: {
			// Notify the server about file changes to '.clientrc files contained in the workspace
			fileEvents: workspace.createFileSystemWatcher('**/.clientrc')
		}
	};

	client = new LanguageClient(
		'gw_basic_server',
		'gw_basic_server',
		serverOptions,
		clientOptions
	);

	// Start the client. This will also launch the server
	// client.start();
	context.subscriptions.push(client.start());
}

