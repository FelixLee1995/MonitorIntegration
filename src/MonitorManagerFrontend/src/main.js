import Vue from 'vue'
import ElementUI from 'element-ui'
import 'element-ui/lib/theme-chalk/index.css'
import App from './App.vue'
import qs from 'qs'
import axios from 'axios'
import VueAxios from 'vue-axios'

Vue.use(ElementUI)
Vue.prototype.$qs = qs

Vue.use(VueAxios, axios)

new Vue({
  el: '#app',
  render: h => h(App)
})
