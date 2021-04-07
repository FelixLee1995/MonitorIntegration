<template>
  <div id="app">

    <el-button type="primary" v-on:click="qry">查询</el-button>
    <el-button type="primary" v-on:click="addRow">新增</el-button>
    <el-button type="primary" v-on:click="checkAll">检查全部</el-button>
    <template>
      <el-radio-group v-model="refresh_interval" @change="changeRefreshInterval">
        <el-radio :label="1">1分钟</el-radio>
        <el-radio :label="10">10分钟</el-radio>
        <el-radio :label="15">15分钟</el-radio>
      </el-radio-group>
    </template>

    <div>
      <el-table
          :data="tableData"
          :key="timeStamp"
          height="500"
          border
          align="center"
      >
        <el-table-column
            prop="appname"
            label="应用名称"
            width="180">
          <template slot-scope="scope">
            <el-input v-model="scope.row.appname" :disabled="!scope.row.type"></el-input>
          </template>
        </el-table-column>
        <el-table-column
            prop="zabbixhost"
            label="zabbixhost"
            width="180">
          <template slot-scope="scope">
            <el-input v-model="scope.row.zabbixhost" :disabled="!scope.row.edit"></el-input>
          </template>
        </el-table-column>
        <el-table-column
            label="地址"
            width="180">
          <template slot-scope="scope">
            <el-input v-model="scope.row.address" :disabled="!scope.row.edit"></el-input>
          </template>
        </el-table-column>
        <el-table-column
            label="端口"
            width="180">
          <template slot-scope="scope">
            <el-input v-model="scope.row.port" :disabled="!scope.row.edit"></el-input>
          </template>
        </el-table-column>
        <el-table-column
            label="状态"
            width="180">
          <template slot-scope="scope">
            <el-button v-show="scope.row.status===''"  size="mini" >未检查</el-button>
            <el-button v-show="scope.row.status==='健康'"  size="mini" type="success">健康</el-button>
            <el-button v-show="scope.row.status==='异常'" size="mini" type="danger">异常</el-button>
            <el-button v-show="scope.row.status==='未知'" size="mini" type="warning">未知</el-button>


          </template>
        </el-table-column>
        <el-table-column
            label="操作"
            width="280">
          <template slot-scope="scope">
            <el-button v-show="scope.row.edit" type="success" size="mini"
                       icon="el-icon-circle-check-outline" @click="confirmEdit(scope.$index, scope.row)">保存
            </el-button>
            <el-button v-show="!scope.row.edit" type="primary" size="mini"
                       @click="startEdit(scope.$index, scope.row)">编辑
            </el-button>
            <el-button v-show="scope.row.edit" class="cancel-btn" size="mini" icon="el-icon-refresh"
                       type="warning" @click="cancelEdit(scope.$index, tableData)">取消
            </el-button>
            <el-button v-show="!scope.row.edit"
                       @click.native.prevent="deleteOne(scope.$index, tableData)"
                       type="text"
                       size="small">
              移除
            </el-button>
            <el-button v-show="!scope.row.edit"
                       @click.native.prevent="healthcheck(scope.$index, scope.row)"
                       type="text"
                       size="small">
              健康检查
            </el-button>
          </template>
        </el-table-column>
      </el-table>
    </div>


    <el-input
        type="textarea"
        id="logid"
        :rows="12"
        placeholder="操作日志"
        v-model="logarea">
    </el-input>
    <el-button @click="clearLog()">清空操作日志</el-button>
  </div>
</template>

<style scoped>
.active {
  color: red;
}
</style>

<script>
export default {
  methods: {
    addRow: function () {
      let newLine = {
        appname: "应用名称",
        zabbixhost: "zabbixhost名称",
        address: "地址",
        port: 0,
        edit: true,
        type: "insert",
        itemid: ""
      };
      if (this.tableData === null) {
        this.tableData = [];
        this.tableData.push(newLine)
      }
      else {
        this.tableData.unshift(newLine) // 移到第一行
      }
    },
    qry: async function (if_checkall) {

      this.axios.post(this.apiUrl + "/api/qrylist").then((response) => {
        if (response.data.data === null) {
          this.messageOK("暂无数据")
        }
        else
          this.tableData = response.data.data;
      }).catch(reason => {
        this.messageFail("qry failed, reason" + reason)
      }).finally(()=>{
        console.log("finally")
        if (if_checkall===true){
          console.log("checkAll")
          this.checkAll()
        }
      })
    },
    startEdit: function (index, tableData) {

      let tempData = this.tableData[index];
      tempData.edit = true;
      tempData.type = null;
      this.$set(this.tableData,index,tempData);
      this.timeStamp = 1
    },
    cancelEdit: function (index, tableData) {
      let tempData = this.tableData[index];
      tempData.edit = false;
      tempData.type = null;
      this.$set(this.tableData,index,tempData);
      this.timeStamp = 1;
      this.qry();
    },
    confirmEdit: function(index, tableData) {

      let reg = /.*[\u4e00-\u9fa5]+.*$/
      if (reg.test(tableData.appname))
      {
        this.messageFail("应用名称不能包含中文字符！")
        return
      }
      let op, url;
      if (tableData.type === "insert") {
        op = "insert"
        url = this.apiUrl + "/api/insert"
      }
      else {
        op = "modify"
        url = this.apiUrl + "/api/modify"
      }

      let para = {
        op: op,
        data: {
          appname: tableData.appname,
          zabbixhost: tableData.zabbixhost,
          address: tableData.address,
          port: Number(tableData.port)
        }
      }
      this.axios.post(url, JSON.stringify(para)).then((response) => {
        console.log("errcode " + response.data.code)

        if(response.data.code === 200){

          this.messageOK("操作成功")
          this.qry()
        }
        else {
          this.messageFail("操作失败")
        }

      })
    },
    messageOK: function (msg) {
      this.$message({
        showClose: true,
        message: msg,
        type: 'success'
      });
    },
    messageFail: function (msg) {
      this.$message({
        showClose: true,
        message: msg,
        type: 'error'
      });
    },
    deleteOne: function(index, tableData) {
      let para = {
        op: "delete",
        data: {
          appname: tableData[index].appname,
          itemid: tableData[index].itemid
        }
      }
      console.log("param is " + JSON.stringify(para))
      this.axios.post(this.apiUrl + "/api/delete", JSON.stringify(para)).then((response) => {
        console.log("delete errcode " + response.data.code)
        if(response.data.code === 200){
          this.messageOK("操作成功")
          this.qry()
      }
      else {
          this.messageFail("操作失败")
        }

      })
    },
    healthcheck: async function (index, tableData) {

      let ret = -1;

      let url = "http://" + tableData.address + ":" + tableData.port + "/healthcheck"
      let log = "检查 【" + tableData.appname + "】, 地址为： " + url

      let para = {
        func_id: 10000,
        data: {
          AppName: tableData.appname
        }
      }

      await this.axios.post(url, JSON.stringify(para), {timeout: 1000}).then((response) => {
        let res = JSON.stringify(response.data);
        console.log("healthcheck res is " + res)
        if (response.data.data.Health === 0) {
          let tempData = this.tableData[index];
          tempData.status = '健康';
          this.$set(this.tableData,index,tempData);
          this.timeStamp = 1
          //this.tableData.empty()
          log += " 结果为：【健康】"
          ret = 1
        } else {
          let tempData = this.tableData[index];
          tempData.status = '异常';
          this.$set(this.tableData,index,tempData);
          this.timeStamp = 1
          log += " 结果为：【异常】, 详细信息： " + res
          ret = 0
        }
        this.logarea += log + "\r\n"
      }, (err) => {
        let tempData = this.tableData[index];
        tempData.status = '未知';
        this.$set(this.tableData,index,tempData);
        this.timeStamp = 1
        log += " 结果为：【未知】, 详细信息： 无法与该应用进行通信"
        ret = -1
        this.logarea += log + "\r\n"

      })

      const textarea = document.getElementById('logid');
      textarea.scrollTop = textarea.scrollHeight;
      return ret
    },
    checkAll: async function () {
      if (this.tableData === null) {
        this.messageOK("无可用监控项")
        return
      }

      let log = ""

      this.logarea += "============开始检查全部============\r\n"
      let idx = 0;
      let error_cnt = 0;
      let success_cnt = 0;
      let unkown_cnt = 0;
      let len = this.tableData.length
      for (let i = 0; i < len; i++) {

        let ret = await this.healthcheck(idx, this.tableData[i])
        if (ret === 1) {
          success_cnt++
        } else if (ret === 0) {
          error_cnt++
        } else {
          unkown_cnt++
        }
        idx++;
      }
      log += "检查完毕, 总计 " + idx + " 个应用， 健康个数 " + success_cnt + ", 异常个数: " + error_cnt + ", 未知个数: " + unkown_cnt + "\r\n"
      log += "检查时间  " + this.dateFormat() + "\r\n"
      log += "============检查结束============\r\n"

      this.logarea += log
      const textarea = document.getElementById('logid');
      textarea.scrollTop = textarea.scrollHeight;
    },
    clearLog: function () {
      this.logarea = ""
    },

    changeRefreshInterval: function (interval){
      console.log("refresh change: " +  interval)
      clearInterval(this.timer)
      this.timer = null
      this.setTimer(interval)
    }
    ,
    setTimer: function (interval) {
      if(this.timer === null) {
        console.log('开始定时...每过' + interval +  '分钟执行一次')
        this.timer = setInterval( () => {
          this.clearLog()
          this.qry(true)
        }, 1000*60* interval)
      }
    },
    dateFormat() {
      let date=new Date();
      let year=date.getFullYear();
      /* 在日期格式中，月份是从0开始的，因此要加0
       * 使用三元表达式在小于10的前面加0，以达到格式统一  如 09:11:05
       * */
      let month= date.getMonth()+1<10 ? "0"+(date.getMonth()+1) : date.getMonth()+1;
      let day=date.getDate()<10 ? "0"+date.getDate() : date.getDate();
      let hours=date.getHours()<10 ? "0"+date.getHours() : date.getHours();
      let minutes=date.getMinutes()<10 ? "0"+date.getMinutes() : date.getMinutes();
      let seconds=date.getSeconds()<10 ? "0"+date.getSeconds() : date.getSeconds();
      // 拼接
      return year+"-"+month+"-"+day+" "+hours+":"+minutes+":"+seconds;
    },
    setApiUrl: function (url) {
      this.apiUrl = url
      this.messageOK("操作成功")
    }

  },
  data() {
    return {
      tableData: [],
      apiUrl: "http://10.0.0.201:18099",
      logarea: "",
      timeStamp: "",
      timer: null,
      refresh_interval: 10
    }
  },
  created() {
    this.qry(true)
    clearInterval(this.timer)
    this.timer = null
    this.setTimer(this.refresh_interval)
  },
  destroyed: function () {
    // 每次离开当前界面时，清除定时器
    clearInterval(this.timer)
    this.timer = null
  }

}
</script>

<style>
#app {
  font-family: Helvetica, sans-serif;
  text-align: center;
}

 .exception {
   color: red;
 }


.healthy {
  color: lawngreen;
}

</style>
