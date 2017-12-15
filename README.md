# BrowserBandit

BrowserBandit searches a browser profile folder (firefox & chrome) and reports on the information found, including user/pass combos, history, autocomplete fields, etc.

*Most functionality for reading firefox browser profiles is now implemented and can be used as follows:*

>./bbandit -p "/path/to/firefox/profile" -b firefox -v

### Progress

- [x] Parse and decrypt Firefox saved credentials
- [x] Retrieve Firefox history and bookmark entries
- [x] Retrieve Firefox AutoComplete form entries

- [ ] ~~Parse and decrypt Chrome saved credentials~~ (See notes)
- [ ] Retrieve Chrome history and bookmark entries
- [ ] Retrieve Chrome AutoComplete form entries

- [ ] Add functionality to save a report of retrieved data **in progress**
- [ ] Create HTML template files for generating an HTML report
- [x] Implement printing a summary report to the console (first 20 items of each category)

### Compiling

In order to compile, you'll need to install the following dependencies:

 > libnss3, libnss3-dev, libsqlite3-0 and libsqlite3-dev

On Debian-based distros, you can run the following command:

    sudo apt-get install libnss3 libnss3-dev libsqlite3-0 libsqlite3-dev

Once the dependencies are installed, just run make to generate an executable.

#### Notes

It appears that Google Chrome uses the Windows Data Protection API to encrypt stored login credentials. This means that the credentials can only be decrypted when logged in in Windows under that user profile, or possibly from any **Windows** account on that same machine. Likewise on Linux, it appears to use Gnome-keyring to store credentials, making it difficult/impossible to decrypt.
