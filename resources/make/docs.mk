.PHONY: docs api-docs user-guide publish clean-docs

DOCS_DIR = $(ROOT_DIR)/docs
USER_GUIDE_DIR = $(DOCS_DIR)/user-guide

# The following dir is for build only and can be destroyed
DOCS_BUILD_DIR = $(DOCS_DIR)/build

# The following dir is tracked by the project git (not the ephemeral git repo
# created for pushing to gh-pages) and CANNOT BE DESTROYED -- this is where the
# versioned docs live (i.e., all past releases of documentation, both API and
# user-guide).
#
# The layout of the archived docs is as follows (relative to DOCS_DIR/master):
#   index.html -> meta-refresh to current
#   current/index.html -> links to current/api/ and current/user-guide/
#   x.y.z/index.html -> links to x.y.z/api/ and x.y.z/user-guide/
#
# Note that /current is not tracked in the repo (there is a .gitignore for
# docs/master/current. Everything else in the layout above, however, is tracked
# in git.
DOCS_PROD_DIR = $(DOCS_DIR)/master

docs: clean-docs api-docs user-guide

publish: docs setup-temp-repo
	@echo "\nPublishing docs ...\n"
	@cd $(DOCS_PROD_DIR) && git push -f $(REPO) master:gh-pages
	@make teardown-temp-repo

# The options for generating the API documentation are saved in configuration
# files. See the project rebar.config file for more info.
api-docs:
	@echo "\nBuilding API docs ...\n"
	@echo "rebar3 lfe lodox"

SLATE_GIT_HACK = $(DOCS_DIR)/.git
CURRENT_VERSION = current

$(SLATE_GIT_HACK):
	-@ln -s $(ROOT_DIR)/.git $(USER_GUIDE_DIR)

user-guide-setup:
	@echo "\nInstalling and setting up dependencies ..."
	@cd $(USER_GUIDE_DIR) && bundle install

user-guide-dev:
	@echo "\nRunning development server ..."
	@cd $(USER_GUIDE_DIR) && bundle exec middleman server

clean-docs:
	@echo "\nCleaning build directory ..."
	@rm -rf $(DOCS_BUILD_DIR)

user-guide: $(SLATE_GIT_HACK)
	@echo "\nBuilding user guide ...\n"
	@cd $(USER_GUIDE_DIR) && bundle exec middleman build --clean

setup-temp-repo: $(SLATE_GIT_HACK)
	@echo "\nSetting up temporary git repos for gh-pages ...\n"
	@rm -rf $(DOCS_PROD_DIR)/$(CURRENT_VERSION) $(DOCS_PROD_DIR)/.git $(DOCS_PROD_DIR)/*/.git
	@cp -r $(DOCS_BUILD_DIR) $(DOCS_PROD_DIR)/$(CURRENT_VERSION)
	@cd $(DOCS_PROD_DIR) && git init
	@cd $(DOCS_PROD_DIR) && git add * > /dev/null
	@cd $(DOCS_PROD_DIR) && git commit -a -m "Generated content." > /dev/null

teardown-temp-repo:
	@echo "\nTearing down temporary gh-pages repos ..."
	@rm $(USER_GUIDE_DIR)/.git $(USER_GUIDE_DIR)/Gemfile.lock
	@rm -rf $(DOCS_PROD_DIR)/.git $(DOCS_PROD_DIR)/*/.git

