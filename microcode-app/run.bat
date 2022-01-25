@echo Starting app server
@docker run --rm -d -v %~dp0data:/app/data -p 3000:3000 microcode-app
@echo Opening browser
@start http://localhost:3000
